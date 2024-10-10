#include "cu_hex8_laplacian.h"

#include "sfem_cuda_base.h"

#ifndef MAX
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#endif


template <typename real_t>
__global__ void cu_affine_hex8_laplacian_apply_kernel(
        const ptrdiff_t nelements,
        const ptrdiff_t stride,  // Stride for elements and fff
        const idx_t *const SFEM_RESTRICT elements,
        const cu_jacobian_t *const SFEM_RESTRICT fff,
        const real_t *const SFEM_RESTRICT x,
        real_t *const SFEM_RESTRICT y) {
    for (ptrdiff_t e = blockIdx.x * blockDim.x + threadIdx.x; e < nelements;
         e += blockDim.x * gridDim.x) {
        // TODO
        assert(false);
    }
}

template <typename T>
static int cu_affine_hex8_laplacian_apply_tpl(
        const ptrdiff_t nelements,
        const ptrdiff_t stride,  // Stride for elements and fff
        const idx_t *const SFEM_RESTRICT elements,
        const cu_jacobian_t *const SFEM_RESTRICT fff,
        const T *const x,
        T *const y,
        void *stream) {
    // Hand tuned
    int block_size = 128;
#ifdef SFEM_USE_OCCUPANCY_MAX_POTENTIAL
    {
        int min_grid_size;
        cudaOccupancyMaxPotentialBlockSize(
                &min_grid_size, &block_size, cu_affine_hex8_laplacian_apply_kernel<T>, 0, 0);
    }
#endif  // SFEM_USE_OCCUPANCY_MAX_POTENTIAL

    const ptrdiff_t n_blocks = MAX(ptrdiff_t(1), (nelements + block_size - 1) / block_size);

    if (stream) {
        cudaStream_t s = *static_cast<cudaStream_t *>(stream);
        cu_affine_hex8_laplacian_apply_kernel<<<n_blocks, block_size, 0, s>>>(
                nelements, stride, elements, fff, x, y);
    } else {
        cu_affine_hex8_laplacian_apply_kernel<<<n_blocks, block_size, 0>>>(
                nelements, stride, elements, fff, x, y);
    }

    return SFEM_SUCCESS;
}

extern int cu_affine_hex8_laplacian_apply(const ptrdiff_t nelements,
                                          const ptrdiff_t stride,  // Stride for elements and fff
                                          const idx_t *const SFEM_RESTRICT elements,
                                          const void *const SFEM_RESTRICT fff,
                                          const enum RealType real_type_xy,
                                          const void *const x,
                                          void *const y,
                                          void *stream) {
    switch (real_type_xy) {
        case SFEM_REAL_DEFAULT: {
            return cu_affine_hex8_laplacian_apply_tpl(nelements,
                                                      stride,
                                                      elements,
                                                      (cu_jacobian_t *)fff,
                                                      (real_t *)x,
                                                      (real_t *)y,
                                                      stream);
        }
        case SFEM_FLOAT32: {
            return cu_affine_hex8_laplacian_apply_tpl(nelements,
                                                      stride,
                                                      elements,
                                                      (cu_jacobian_t *)fff,
                                                      (float *)x,
                                                      (float *)y,
                                                      stream);
        }
        case SFEM_FLOAT64: {
            return cu_affine_hex8_laplacian_apply_tpl(nelements,
                                                      stride,
                                                      elements,
                                                      (cu_jacobian_t *)fff,
                                                      (double *)x,
                                                      (double *)y,
                                                      stream);
        }
        default: {
            fprintf(stderr,
                    "[Error] cu_hex8_laplacian_apply: not implemented for type %s (code %d)\n",
                    real_type_to_string(real_type_xy),
                    real_type_xy);
            assert(0);
            return SFEM_FAILURE;
        }
    }
}
