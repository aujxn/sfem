#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_dtof.h"
#include "matrixio_array.h"
#include "matrixio_crs.h"
#include "utils.h"

#include "crs_graph.h"
#include "sfem_base.h"

#include "read_mesh.h"

#include "operators/grad_p1.h"

static SFEM_INLINE void outflux_kernel(const real_t px0,
                                       const real_t px1,
                                       const real_t px2,
                                       const real_t py0,
                                       const real_t py1,
                                       const real_t py2,
                                       const real_t pz0,
                                       const real_t pz1,
                                       const real_t pz2,
                                       // Normals
                                       const real_t nx,
                                       const real_t ny,
                                       const real_t nz,
                                       // Data
                                       const real_t *const SFEM_RESTRICT vx,
                                       const real_t *const SFEM_RESTRICT vy,
                                       const real_t *const SFEM_RESTRICT vz,
                                       // Output
                                       real_t *const SFEM_RESTRICT value) {
    // 
}

static SFEM_INLINE void normalize(real_t *const vec3) {
    const real_t len = sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);
    vec3[0] /= len;
    vec3[1] /= len;
    vec3[2] /= len;
}

void normals(const ptrdiff_t nelements,
             const ptrdiff_t nnodes,
             idx_t **const elems,
             geom_t **const xyz,
             geom_t **const normals_xyz) {
    SFEM_UNUSED(nnodes);

    double tick = MPI_Wtime();

    idx_t ev[4];

    for (ptrdiff_t i = 0; i < nelements; ++i) {
#pragma unroll(4)
        for (int v = 0; v < 3; ++v) {
            ev[v] = elems[v][i];
        }

        // Element indices
        const idx_t i0 = ev[0];
        const idx_t i1 = ev[1];
        const idx_t i2 = ev[2];

        real_t u[3] = {xyz[0][i1] - xyz[0][i0], xyz[1][i1] - xyz[1][i0], xyz[2][i1] - xyz[2][i0]};
        real_t v[3] = {xyz[0][i2] - xyz[0][i0], xyz[1][i2] - xyz[1][i0], xyz[2][i2] - xyz[2][i0]};

        normalize(u);
        normalize(v);

        real_t n[3] = {u[1] * v[2] - u[2] * v[1],  //
                             u[2] * v[0] - u[0] * v[2],  //
                             u[0] * v[1] - u[1] * v[0]};

        normalize(n);

        normals_xyz[0][i] = n[0];
        normals_xyz[1][i] = n[1];
        normals_xyz[2][i] = n[2];
    }

    double tock = MPI_Wtime();
    printf("surface_outflux.c: normals\t%g seconds\n", tock - tick);
}

void surface_outflux(const ptrdiff_t nelements,
                     const ptrdiff_t nnodes,
                     idx_t **const elems,
                     geom_t **const xyz,
                     geom_t **const normals_xyz,
                     real_t *const vector_field_x,
                     real_t *const vector_field_y,
                     real_t *const vector_field_z,
                     real_t *const value) {
    SFEM_UNUSED(nnodes);

    double tick = MPI_Wtime();

    real_t element_vector_x[3];
    real_t element_vector_y[3];
    real_t element_vector_z[3];

    *value = 0;

    idx_t ev[4];

    for (ptrdiff_t i = 0; i < nelements; ++i) {
#pragma unroll(4)
        for (int v = 0; v < 3; ++v) {
            ev[v] = elems[v][i];
        }

        // Global to local
        for (int v = 0; v < 3; ++v) {
            element_vector_x[v] = vector_field_x[ev[v]];
            element_vector_y[v] = vector_field_y[ev[v]];
            element_vector_z[v] = vector_field_z[ev[v]];
        }

        // Element indices
        const idx_t i0 = ev[0];
        const idx_t i1 = ev[1];
        const idx_t i2 = ev[2];
        const idx_t i3 = ev[3];

        real_t vector_field[6];
        vector_field[0] = vector_field_x[i];
        vector_field[1] = vector_field_y[i];
        vector_field[2] = vector_field_z[i];

        real_t element_value = 0;

        outflux_kernel(
            // X-coordinates
            xyz[0][i0],
            xyz[0][i1],
            xyz[0][i2],
            // Y-coordinates
            xyz[1][i0],
            xyz[1][i1],
            xyz[1][i2],
            // Z-coordinates
            xyz[2][i0],
            xyz[2][i1],
            xyz[2][i2],
            // Normal,
            normals_xyz[0][i],
            normals_xyz[1][i],
            normals_xyz[2][i],
            // Data
            element_vector_x,
            element_vector_y,
            element_vector_z,
            // Output
            &element_value);

        *value += element_value;
    }

    double tock = MPI_Wtime();
    printf("surface_outflux.c: surface_outflux\t%g seconds\n", tock - tick);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    MPI_Comm comm = MPI_COMM_WORLD;

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (size != 1) {
        fprintf(stderr, "Parallel execution not supported!\n");
        return EXIT_FAILURE;
    }

    if (argc != 6) {
        fprintf(stderr, "usage: %s <folder> <vx.raw> <vy.raw> <vz.raw> <outflux.raw>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *folder = argv[1];
    const char *path_vector_field[3] = {argv[2], argv[3], argv[4]};
    const char *path_output = argv[5];

    printf("%s %s %s %s %s %s\n",
           argv[0],
           folder,
           path_vector_field[0],
           path_vector_field[1],
           path_vector_field[2],
           path_output);

    double tick = MPI_Wtime();

    ///////////////////////////////////////////////////////////////////////////////
    // Read data
    ///////////////////////////////////////////////////////////////////////////////

    mesh_t mesh;
    if (mesh_surf_read(comm, folder, &mesh)) {
        return EXIT_FAILURE;
    }

    real_t *vector_field[3];
    ptrdiff_t vector_field_size_local, vector_field_size_global;

    for (int d = 0; d < 3; ++d) {
        array_create_from_file(comm,
                               path_vector_field[d],
                               SFEM_MPI_REAL_T,
                               (void **)&vector_field[d],
                               &vector_field_size_local,
                               &vector_field_size_global);
    }

    // integral over surface
    // TODO

    geom_t *normals_xyz[3];

    for (int d = 0; d < 3; ++d) {
        normals_xyz[d] = (geom_t *)malloc(mesh.nelements * sizeof(geom_t));
    }

    normals(mesh.nelements, mesh.nnodes, mesh.elements, mesh.points, normals_xyz);

    real_t value = 0;
    surface_outflux(mesh.nelements,
                    mesh.nnodes,
                    mesh.elements,
                    mesh.points,
                    normals_xyz,
                    vector_field[0],
                    vector_field[1],
                    vector_field[2],
                    &value);

    printf("surface_outflux = %g\n", (double)value);

    array_write(comm, "normalx.raw", SFEM_MPI_GEOM_T, normals_xyz[0], mesh.nelements, mesh.nelements);
    array_write(comm, "normaly.raw", SFEM_MPI_GEOM_T, normals_xyz[1], mesh.nelements, mesh.nelements);
    array_write(comm, "normalz.raw", SFEM_MPI_GEOM_T, normals_xyz[2], mesh.nelements, mesh.nelements);

    for (int d = 0; d < 3; ++d) {
        free(normals_xyz[d]);
        free(vector_field[d]);
    }

    double tock = MPI_Wtime();

    if (!rank) {
        printf("----------------------------------------\n");
        printf("#elements %ld #nodes %ld\n", (long)mesh.nelements, (long)mesh.nnodes);
        printf("TTS:\t\t\t%g seconds\n", tock - tick);
    }

    return MPI_Finalize();
}
