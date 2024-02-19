#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "matrixio_array.h"
#include "matrixio_crs.h"
#include "utils.h"

#include "dirichlet.h"

#include "sfem_base.h"
#include "spmv.h"

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
        fprintf(
            stderr, "usage: %s <alpha> <transpose> <crs_folder> <x.raw> <output.raw>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const real_t alpha = atof(argv[1]);
    const int transpose = atoi(argv[2]);
    const char *crs_folder = argv[3];
    const char *x_path = argv[4];
    const char *output_path = argv[5];

    double tick = MPI_Wtime();

    char rowptr_path[1024 * 10];
    char colidx_path[1024 * 10];
    char values_path[1024 * 10];

    sprintf(rowptr_path, "%s/rowptr.raw", crs_folder);
    sprintf(colidx_path, "%s/colidx.raw", crs_folder);
    sprintf(values_path, "%s/values.raw", crs_folder);

    crs_t crs;
    crs_read(comm,
             rowptr_path,
             colidx_path,
             values_path,
             SFEM_MPI_COUNT_T,
             SFEM_MPI_IDX_T,
             SFEM_MPI_REAL_T,
             &crs);

    ptrdiff_t _nope_, x_n;
    real_t *x = 0;
    array_create_from_file(comm, x_path, SFEM_MPI_REAL_T, (void **)&x, &_nope_, &x_n);

    real_t *y = calloc(crs.grows, sizeof(real_t));

    double spmv_tick = MPI_Wtime();

    scal(x_n, alpha, x);
    crs_spmv(crs.grows,
             (const count_t *const)crs.rowptr,
             (const idx_t *const)crs.colidx,
             (const real_t *const)crs.values,
             x,
             y);

    double spmv_tock = MPI_Wtime();
    printf("spmv: %g (seconds)\n", spmv_tock - spmv_tick);

    array_write(comm, output_path, SFEM_MPI_REAL_T, y, crs.grows, crs.grows);
    crs_free(&crs);
    free(x);
    free(y);

    double tock = MPI_Wtime();
    if (!rank) {
        printf("do_spmv.c\n");
        printf("TTS: %g seconds\n", tock - tick);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
