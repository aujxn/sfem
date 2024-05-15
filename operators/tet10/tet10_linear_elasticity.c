#include "tet10_linear_elasticity.h"
#include <stddef.h>
#include "sfem_base.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define POW2(a) ((a) * (a))

#ifdef SFEM_ENABLE_FP32_KERNELS
typedef float scalar_t;
#else
typedef real_t scalar_t;
#endif

typedef scalar_t accumulator_t;

static SFEM_INLINE void jacobian_micro_kernel(const geom_t px0,
                                              const geom_t px1,
                                              const geom_t px2,
                                              const geom_t px3,
                                              const geom_t py0,
                                              const geom_t py1,
                                              const geom_t py2,
                                              const geom_t py3,
                                              const geom_t pz0,
                                              const geom_t pz1,
                                              const geom_t pz2,
                                              const geom_t pz3,
                                              jacobian_t *jacobian) {
    jacobian[0] = -px0 + px1;
    jacobian[1] = -px0 + px2;
    jacobian[2] = -px0 + px3;
    jacobian[3] = -py0 + py1;
    jacobian[4] = -py0 + py2;
    jacobian[5] = -py0 + py3;
    jacobian[6] = -pz0 + pz1;
    jacobian[7] = -pz0 + pz2;
    jacobian[8] = -pz0 + pz3;
}

static SFEM_INLINE void adjugate_and_det_micro_kernel(const geom_t px0,
                                                      const geom_t px1,
                                                      const geom_t px2,
                                                      const geom_t px3,
                                                      const geom_t py0,
                                                      const geom_t py1,
                                                      const geom_t py2,
                                                      const geom_t py3,
                                                      const geom_t pz0,
                                                      const geom_t pz1,
                                                      const geom_t pz2,
                                                      const geom_t pz3,
                                                      jacobian_t *adjugate,
                                                      jacobian_t *jacobian_determinant) {
    // Compute jacobian in high precision
    real_t jacobian[9];
    jacobian[0] = -px0 + px1;
    jacobian[1] = -px0 + px2;
    jacobian[2] = -px0 + px3;
    jacobian[3] = -py0 + py1;
    jacobian[4] = -py0 + py2;
    jacobian[5] = -py0 + py3;
    jacobian[6] = -pz0 + pz1;
    jacobian[7] = -pz0 + pz2;
    jacobian[8] = -pz0 + pz3;

    const real_t x0 = jacobian[4] * jacobian[8];
    const real_t x1 = jacobian[5] * jacobian[7];
    const real_t x2 = jacobian[1] * jacobian[8];
    const real_t x3 = jacobian[1] * jacobian[5];
    const real_t x4 = jacobian[2] * jacobian[4];

    // Store adjugate in lower precision
    adjugate[0] = x0 - x1;
    adjugate[1] = jacobian[2] * jacobian[7] - x2;
    adjugate[2] = x3 - x4;
    adjugate[3] = -jacobian[3] * jacobian[8] + jacobian[5] * jacobian[6];
    adjugate[4] = jacobian[0] * jacobian[8] - jacobian[2] * jacobian[6];
    adjugate[5] = -jacobian[0] * jacobian[5] + jacobian[2] * jacobian[3];
    adjugate[6] = jacobian[3] * jacobian[7] - jacobian[4] * jacobian[6];
    adjugate[7] = -jacobian[0] * jacobian[7] + jacobian[1] * jacobian[6];
    adjugate[8] = jacobian[0] * jacobian[4] - jacobian[1] * jacobian[3];

    // Store determinant in lower precision
    jacobian_determinant[0] = jacobian[0] * x0 - jacobian[0] * x1 +
                              jacobian[2] * jacobian[3] * jacobian[7] - jacobian[3] * x2 +
                              jacobian[6] * x3 - jacobian[6] * x4;
}

static SFEM_INLINE void ref_shape_grad_x(const scalar_t qx,
                                         const scalar_t qy,
                                         const scalar_t qz,
                                         scalar_t *const out) {
    const scalar_t x0 = 4 * qx;
    const scalar_t x1 = 4 * qy;
    const scalar_t x2 = 4 * qz;
    const scalar_t x3 = x1 + x2;
    out[0] = x0 + x3 - 3;
    out[1] = x0 - 1;
    out[2] = 0;
    out[3] = 0;
    out[4] = -8 * qx - x3 + 4;
    out[5] = x1;
    out[6] = -x1;
    out[7] = -x2;
    out[8] = x2;
    out[9] = 0;
}

static SFEM_INLINE void ref_shape_grad_y(const scalar_t qx,
                                         const scalar_t qy,
                                         const scalar_t qz,
                                         scalar_t *const out) {
    const scalar_t x0 = 4 * qy;
    const scalar_t x1 = 4 * qx;
    const scalar_t x2 = 4 * qz;
    const scalar_t x3 = x1 + x2;
    out[0] = x0 + x3 - 3;
    out[1] = 0;
    out[2] = x0 - 1;
    out[3] = 0;
    out[4] = -x1;
    out[5] = x1;
    out[6] = -8 * qy - x3 + 4;
    out[7] = -x2;
    out[8] = 0;
    out[9] = x2;
}

static SFEM_INLINE void ref_shape_grad_z(const scalar_t qx,
                                         const scalar_t qy,
                                         const scalar_t qz,
                                         scalar_t *const out) {
    const scalar_t x0 = 4 * qz;
    const scalar_t x1 = 4 * qx;
    const scalar_t x2 = 4 * qy;
    const scalar_t x3 = x1 + x2;
    out[0] = x0 + x3 - 3;
    out[1] = 0;
    out[2] = 0;
    out[3] = x0 - 1;
    out[4] = -x1;
    out[5] = 0;
    out[6] = -x2;
    out[7] = -8 * qz - x3 + 4;
    out[8] = x1;
    out[9] = x2;
}

static SFEM_INLINE void tet10_linear_elasticity_apply_micro_kernel(
    const scalar_t mu,
    const scalar_t lambda,
    const jacobian_t *const SFEM_RESTRICT adjugate,
    const jacobian_t jacobian_determinant,
    const scalar_t qx,
    const scalar_t qy,
    const scalar_t qz,
    const scalar_t qw,
    const scalar_t *const SFEM_RESTRICT u,
    scalar_t *const SFEM_RESTRICT element_vector) {
    // This can be reduced with 1D products (ref_shape_grad_{x,y,z})
    real_t disp_grad[9] = {0};

#define MICRO_KERNEL_USE_CODEGEN 1

#if MICRO_KERNEL_USE_CODEGEN
    // Code-gen way

    const scalar_t denom = 1;
    {
        const scalar_t x0 = 1.0 / jacobian_determinant;
        const scalar_t x1 = 4 * qx;
        const scalar_t x2 = x1 - 1;
        const scalar_t x3 = 4 * qy;
        const scalar_t x4 = -u[6] * x3;
        const scalar_t x5 = qz - 1;
        const scalar_t x6 = 8 * qx + 4 * qy + 4 * x5;
        const scalar_t x7 = 4 * qz;
        const scalar_t x8 = x1 + x3 + x7 - 3;
        const scalar_t x9 = u[0] * x8;
        const scalar_t x10 = -u[7] * x7 + x9;
        const scalar_t x11 = u[1] * x2 - u[4] * x6 + u[5] * x3 + u[8] * x7 + x10 + x4;
        const scalar_t x12 = x3 - 1;
        const scalar_t x13 = -u[4] * x1;
        const scalar_t x14 = 4 * qx + 8 * qy + 4 * x5;
        const scalar_t x15 = u[2] * x12 + u[5] * x1 - u[6] * x14 + u[9] * x7 + x10 + x13;
        const scalar_t x16 = x7 - 1;
        const scalar_t x17 = 4 * qx + 4 * qy + 8 * qz - 4;
        const scalar_t x18 = u[3] * x16 - u[7] * x17 + u[8] * x1 + u[9] * x3 + x13 + x4 + x9;
        const scalar_t x19 = -u[16] * x3;
        const scalar_t x20 = u[10] * x8;
        const scalar_t x21 = -u[17] * x7 + x20;
        const scalar_t x22 = u[11] * x2 - u[14] * x6 + u[15] * x3 + u[18] * x7 + x19 + x21;
        const scalar_t x23 = -u[14] * x1;
        const scalar_t x24 = u[12] * x12 + u[15] * x1 - u[16] * x14 + u[19] * x7 + x21 + x23;
        const scalar_t x25 = u[13] * x16 - u[17] * x17 + u[18] * x1 + u[19] * x3 + x19 + x20 + x23;
        const scalar_t x26 = -u[26] * x3;
        const scalar_t x27 = u[20] * x8;
        const scalar_t x28 = -u[27] * x7 + x27;
        const scalar_t x29 = u[21] * x2 - u[24] * x6 + u[25] * x3 + u[28] * x7 + x26 + x28;
        const scalar_t x30 = -u[24] * x1;
        const scalar_t x31 = u[22] * x12 + u[25] * x1 - u[26] * x14 + u[29] * x7 + x28 + x30;
        const scalar_t x32 = u[23] * x16 - u[27] * x17 + u[28] * x1 + u[29] * x3 + x26 + x27 + x30;
        disp_grad[0] = x0 * (adjugate[0] * x11 + adjugate[3] * x15 + adjugate[6] * x18);
        disp_grad[1] = x0 * (adjugate[1] * x11 + adjugate[4] * x15 + adjugate[7] * x18);
        disp_grad[2] = x0 * (adjugate[2] * x11 + adjugate[5] * x15 + adjugate[8] * x18);
        disp_grad[3] = x0 * (adjugate[0] * x22 + adjugate[3] * x24 + adjugate[6] * x25);
        disp_grad[4] = x0 * (adjugate[1] * x22 + adjugate[4] * x24 + adjugate[7] * x25);
        disp_grad[5] = x0 * (adjugate[2] * x22 + adjugate[5] * x24 + adjugate[8] * x25);
        disp_grad[6] = x0 * (adjugate[0] * x29 + adjugate[3] * x31 + adjugate[6] * x32);
        disp_grad[7] = x0 * (adjugate[1] * x29 + adjugate[4] * x31 + adjugate[7] * x32);
        disp_grad[8] = x0 * (adjugate[2] * x29 + adjugate[5] * x31 + adjugate[8] * x32);
    }
#else
    // Programmatic way

    const scalar_t denom = jacobian_determinant;
    {
        scalar_t temp[9] = {0};
        scalar_t grad[10];

        ref_shape_grad_x(qx, qy, qz, grad);
#pragma unroll
        for (int i = 0; i < 10; i++) {
            const scalar_t g = grad[i];
            temp[0] += u[i] * g;
            temp[3] += u[10 + i] * g;
            temp[6] += u[20 + i] * g;
        }

        ref_shape_grad_y(qx, qy, qz, grad);
#pragma unroll
        for (int i = 0; i < 10; i++) {
            const scalar_t g = grad[i];
            temp[1] += u[i] * g;
            temp[4] += u[10 + i] * g;
            temp[7] += u[20 + i] * g;
        }

        ref_shape_grad_z(qx, qy, qz, grad);
#pragma unroll
        for (int i = 0; i < 10; i++) {
            const scalar_t g = grad[i];
            temp[2] += u[i] * g;
            temp[5] += u[10 + i] * g;
            temp[8] += u[20 + i] * g;
        }

        for (int i = 0; i < 3; i++) {
#pragma unroll
            for (int j = 0; j < 3; j++) {
#pragma unroll
                for (int k = 0; k < 3; k++) {
                    disp_grad[i * 3 + j] += temp[i * 3 + k] * adjugate[k * 3 + j];
                }
            }
        }
    }

#endif
    // Includes first Piola-Kirchoff stress: P^T * J^-T * det(J)

    scalar_t *P_tXJinv_t = disp_grad;
    {
        const scalar_t x0 = (1.0 / 6.0) * mu;
        const scalar_t x1 = x0 * (disp_grad[1] + disp_grad[3]);
        const scalar_t x2 = x0 * (disp_grad[2] + disp_grad[6]);
        const scalar_t x3 = 2 * mu;
        const scalar_t x4 = lambda * (disp_grad[0] + disp_grad[4] + disp_grad[8]);
        const scalar_t x5 = (1.0 / 6.0) * disp_grad[0] * x3 + (1.0 / 6.0) * x4;
        const scalar_t x6 = x0 * (disp_grad[5] + disp_grad[7]);
        const scalar_t x7 = (1.0 / 6.0) * disp_grad[4] * x3 + (1.0 / 6.0) * x4;
        const scalar_t x8 = (1.0 / 6.0) * disp_grad[8] * x3 + (1.0 / 6.0) * x4;
        P_tXJinv_t[0] = adjugate[0] * x5 + adjugate[1] * x1 + adjugate[2] * x2;
        P_tXJinv_t[1] = adjugate[3] * x5 + adjugate[4] * x1 + adjugate[5] * x2;
        P_tXJinv_t[2] = adjugate[6] * x5 + adjugate[7] * x1 + adjugate[8] * x2;
        P_tXJinv_t[3] = adjugate[0] * x1 + adjugate[1] * x7 + adjugate[2] * x6;
        P_tXJinv_t[4] = adjugate[3] * x1 + adjugate[4] * x7 + adjugate[5] * x6;
        P_tXJinv_t[5] = adjugate[6] * x1 + adjugate[7] * x7 + adjugate[8] * x6;
        P_tXJinv_t[6] = adjugate[0] * x2 + adjugate[1] * x6 + adjugate[2] * x8;
        P_tXJinv_t[7] = adjugate[3] * x2 + adjugate[4] * x6 + adjugate[5] * x8;
        P_tXJinv_t[8] = adjugate[6] * x2 + adjugate[7] * x6 + adjugate[8] * x8;
    }

    // Scale by quadrature weight
    for (int i = 0; i < 9; i++) {
        P_tXJinv_t[i] *= qw / denom;
    }

// On CPU both versions are equivalent
#if MICRO_KERNEL_USE_CODEGEN
    {
        const scalar_t x0 = 4 * qx;
        const scalar_t x1 = 4 * qy;
        const scalar_t x2 = 4 * qz;
        const scalar_t x3 = x0 + x1 + x2 - 3;
        const scalar_t x4 = x0 - 1;
        const scalar_t x5 = x1 - 1;
        const scalar_t x6 = x2 - 1;
        const scalar_t x7 = P_tXJinv_t[1] * x0;
        const scalar_t x8 = P_tXJinv_t[2] * x0;
        const scalar_t x9 = qz - 1;
        const scalar_t x10 = 8 * qx + 4 * qy + 4 * x9;
        const scalar_t x11 = P_tXJinv_t[0] * x1;
        const scalar_t x12 = P_tXJinv_t[2] * x1;
        const scalar_t x13 = 4 * qx + 8 * qy + 4 * x9;
        const scalar_t x14 = P_tXJinv_t[0] * x2;
        const scalar_t x15 = P_tXJinv_t[1] * x2;
        const scalar_t x16 = 4 * qx + 4 * qy + 8 * qz - 4;
        const scalar_t x17 = P_tXJinv_t[4] * x0;
        const scalar_t x18 = P_tXJinv_t[5] * x0;
        const scalar_t x19 = P_tXJinv_t[3] * x1;
        const scalar_t x20 = P_tXJinv_t[5] * x1;
        const scalar_t x21 = P_tXJinv_t[3] * x2;
        const scalar_t x22 = P_tXJinv_t[4] * x2;
        const scalar_t x23 = P_tXJinv_t[7] * x0;
        const scalar_t x24 = P_tXJinv_t[8] * x0;
        const scalar_t x25 = P_tXJinv_t[6] * x1;
        const scalar_t x26 = P_tXJinv_t[8] * x1;
        const scalar_t x27 = P_tXJinv_t[6] * x2;
        const scalar_t x28 = P_tXJinv_t[7] * x2;
        element_vector[0] += x3 * (P_tXJinv_t[0] + P_tXJinv_t[1] + P_tXJinv_t[2]);
        element_vector[1] += P_tXJinv_t[0] * x4;
        element_vector[2] += P_tXJinv_t[1] * x5;
        element_vector[3] += P_tXJinv_t[2] * x6;
        element_vector[4] += -P_tXJinv_t[0] * x10 - x7 - x8;
        element_vector[5] += x11 + x7;
        element_vector[6] += -P_tXJinv_t[1] * x13 - x11 - x12;
        element_vector[7] += -P_tXJinv_t[2] * x16 - x14 - x15;
        element_vector[8] += x14 + x8;
        element_vector[9] += x12 + x15;
        element_vector[10] += x3 * (P_tXJinv_t[3] + P_tXJinv_t[4] + P_tXJinv_t[5]);
        element_vector[11] += P_tXJinv_t[3] * x4;
        element_vector[12] += P_tXJinv_t[4] * x5;
        element_vector[13] += P_tXJinv_t[5] * x6;
        element_vector[14] += -P_tXJinv_t[3] * x10 - x17 - x18;
        element_vector[15] += x17 + x19;
        element_vector[16] += -P_tXJinv_t[4] * x13 - x19 - x20;
        element_vector[17] += -P_tXJinv_t[5] * x16 - x21 - x22;
        element_vector[18] += x18 + x21;
        element_vector[19] += x20 + x22;
        element_vector[20] += x3 * (P_tXJinv_t[6] + P_tXJinv_t[7] + P_tXJinv_t[8]);
        element_vector[21] += P_tXJinv_t[6] * x4;
        element_vector[22] += P_tXJinv_t[7] * x5;
        element_vector[23] += P_tXJinv_t[8] * x6;
        element_vector[24] += -P_tXJinv_t[6] * x10 - x23 - x24;
        element_vector[25] += x23 + x25;
        element_vector[26] += -P_tXJinv_t[7] * x13 - x25 - x26;
        element_vector[27] += -P_tXJinv_t[8] * x16 - x27 - x28;
        element_vector[28] += x24 + x27;
        element_vector[29] += x26 + x28;
    }

#else

    {
        scalar_t grad[10];
        ref_shape_grad_x(qx, qy, qz, grad);

#pragma unroll
        for (int i = 0; i < 10; i++) {
            scalar_t g = grad[i];
            element_vector[i] += P_tXJinv_t[0] * g;
            element_vector[10 + i] += P_tXJinv_t[3] * g;
            element_vector[20 + i] += P_tXJinv_t[6] * g;
        }

        ref_shape_grad_y(qx, qy, qz, grad);

#pragma unroll
        for (int i = 0; i < 10; i++) {
            scalar_t g = grad[i];
            element_vector[i] += P_tXJinv_t[1] * g;
            element_vector[10 + i] += P_tXJinv_t[4] * g;
            element_vector[20 + i] += P_tXJinv_t[7] * g;
        }

        ref_shape_grad_z(qx, qy, qz, grad);

#pragma unroll
        for (int i = 0; i < 10; i++) {
            scalar_t g = grad[i];
            element_vector[i] += P_tXJinv_t[2] * g;
            element_vector[10 + i] += P_tXJinv_t[5] * g;
            element_vector[20 + i] += P_tXJinv_t[8] * g;
        }
    }

#endif

#undef MICRO_KERNEL_USE_CODEGEN
}

static SFEM_INLINE void diag_micro_kernel(const scalar_t mu,
                                          const scalar_t lambda,
                                          const jacobian_t *const SFEM_RESTRICT adjugate,
                                          const jacobian_t jacobian_determinant,
                                          const scalar_t qx,
                                          const scalar_t qy,
                                          const scalar_t qz,
                                          const scalar_t qw,
                                          accumulator_t *const SFEM_RESTRICT diag) {
    const scalar_t x0 = POW2(adjugate[1] + adjugate[4] + adjugate[7]);
    const scalar_t x1 = mu * x0;
    const scalar_t x2 = POW2(adjugate[2] + adjugate[5] + adjugate[8]);
    const scalar_t x3 = mu * x2;
    const scalar_t x4 = lambda + 2 * mu;
    const scalar_t x5 = POW2(adjugate[0] + adjugate[3] + adjugate[6]);
    const scalar_t x6 = 4 * qx;
    const scalar_t x7 = 4 * qy;
    const scalar_t x8 = 4 * qz;
    const scalar_t x9 = 1.0 / jacobian_determinant;
    const scalar_t x10 = (1.0 / 6.0) * x9;
    const scalar_t x11 = x10 * POW2(x6 + x7 + x8 - 3);
    const scalar_t x12 = POW2(adjugate[1]);
    const scalar_t x13 = mu * x12;
    const scalar_t x14 = POW2(adjugate[2]);
    const scalar_t x15 = mu * x14;
    const scalar_t x16 = POW2(adjugate[0]);
    const scalar_t x17 = x10 * POW2(x6 - 1);
    const scalar_t x18 = POW2(adjugate[4]);
    const scalar_t x19 = mu * x18;
    const scalar_t x20 = POW2(adjugate[5]);
    const scalar_t x21 = mu * x20;
    const scalar_t x22 = POW2(adjugate[3]);
    const scalar_t x23 = x10 * POW2(x7 - 1);
    const scalar_t x24 = POW2(adjugate[7]);
    const scalar_t x25 = mu * x24;
    const scalar_t x26 = POW2(adjugate[8]);
    const scalar_t x27 = mu * x26;
    const scalar_t x28 = POW2(adjugate[6]);
    const scalar_t x29 = x10 * POW2(x8 - 1);
    const scalar_t x30 = adjugate[4] * qx;
    const scalar_t x31 = adjugate[7] * qx;
    const scalar_t x32 = qz - 1;
    const scalar_t x33 = 2 * qx + qy + x32;
    const scalar_t x34 = POW2(adjugate[1] * x33 + x30 + x31);
    const scalar_t x35 = mu * x34;
    const scalar_t x36 = adjugate[5] * qx;
    const scalar_t x37 = adjugate[8] * qx;
    const scalar_t x38 = POW2(adjugate[2] * x33 + x36 + x37);
    const scalar_t x39 = mu * x38;
    const scalar_t x40 = adjugate[3] * qx;
    const scalar_t x41 = adjugate[6] * qx;
    const scalar_t x42 = POW2(adjugate[0] * x33 + x40 + x41);
    const scalar_t x43 = (8.0 / 3.0) * x9;
    const scalar_t x44 = adjugate[1] * qy;
    const scalar_t x45 = POW2(x30 + x44);
    const scalar_t x46 = mu * x45;
    const scalar_t x47 = adjugate[2] * qy;
    const scalar_t x48 = POW2(x36 + x47);
    const scalar_t x49 = mu * x48;
    const scalar_t x50 = adjugate[0] * qy;
    const scalar_t x51 = POW2(x40 + x50);
    const scalar_t x52 = adjugate[7] * qy;
    const scalar_t x53 = qx + 2 * qy + x32;
    const scalar_t x54 = POW2(adjugate[4] * x53 + x44 + x52);
    const scalar_t x55 = mu * x54;
    const scalar_t x56 = adjugate[8] * qy;
    const scalar_t x57 = POW2(adjugate[5] * x53 + x47 + x56);
    const scalar_t x58 = mu * x57;
    const scalar_t x59 = adjugate[6] * qy;
    const scalar_t x60 = POW2(adjugate[3] * x53 + x50 + x59);
    const scalar_t x61 = adjugate[1] * qz;
    const scalar_t x62 = adjugate[4] * qz;
    const scalar_t x63 = qx + qy + 2 * qz - 1;
    const scalar_t x64 = POW2(adjugate[7] * x63 + x61 + x62);
    const scalar_t x65 = mu * x64;
    const scalar_t x66 = adjugate[2] * qz;
    const scalar_t x67 = adjugate[5] * qz;
    const scalar_t x68 = POW2(adjugate[8] * x63 + x66 + x67);
    const scalar_t x69 = mu * x68;
    const scalar_t x70 = adjugate[0] * qz;
    const scalar_t x71 = adjugate[3] * qz;
    const scalar_t x72 = POW2(adjugate[6] * x63 + x70 + x71);
    const scalar_t x73 = POW2(x31 + x61);
    const scalar_t x74 = mu * x73;
    const scalar_t x75 = POW2(x37 + x66);
    const scalar_t x76 = mu * x75;
    const scalar_t x77 = POW2(x41 + x70);
    const scalar_t x78 = POW2(x52 + x62);
    const scalar_t x79 = mu * x78;
    const scalar_t x80 = POW2(x56 + x67);
    const scalar_t x81 = mu * x80;
    const scalar_t x82 = POW2(x59 + x71);
    const scalar_t x83 = mu * x5;
    const scalar_t x84 = mu * x16;
    const scalar_t x85 = mu * x22;
    const scalar_t x86 = mu * x28;
    const scalar_t x87 = mu * x42;
    const scalar_t x88 = mu * x51;
    const scalar_t x89 = mu * x60;
    const scalar_t x90 = mu * x72;
    const scalar_t x91 = mu * x77;
    const scalar_t x92 = mu * x82;
    diag[0] += x11 * (x1 + x3 + x4 * x5);
    diag[1] += x17 * (x13 + x15 + x16 * x4);
    diag[2] += x23 * (x19 + x21 + x22 * x4);
    diag[3] += x29 * (x25 + x27 + x28 * x4);
    diag[4] += x43 * (x35 + x39 + x4 * x42);
    diag[5] += x43 * (x4 * x51 + x46 + x49);
    diag[6] += x43 * (x4 * x60 + x55 + x58);
    diag[7] += x43 * (x4 * x72 + x65 + x69);
    diag[8] += x43 * (x4 * x77 + x74 + x76);
    diag[9] += x43 * (x4 * x82 + x79 + x81);
    diag[10] += x11 * (x0 * x4 + x3 + x83);
    diag[11] += x17 * (x12 * x4 + x15 + x84);
    diag[12] += x23 * (x18 * x4 + x21 + x85);
    diag[13] += x29 * (x24 * x4 + x27 + x86);
    diag[14] += x43 * (x34 * x4 + x39 + x87);
    diag[15] += x43 * (x4 * x45 + x49 + x88);
    diag[16] += x43 * (x4 * x54 + x58 + x89);
    diag[17] += x43 * (x4 * x64 + x69 + x90);
    diag[18] += x43 * (x4 * x73 + x76 + x91);
    diag[19] += x43 * (x4 * x78 + x81 + x92);
    diag[20] += x11 * (x1 + x2 * x4 + x83);
    diag[21] += x17 * (x13 + x14 * x4 + x84);
    diag[22] += x23 * (x19 + x20 * x4 + x85);
    diag[23] += x29 * (x25 + x26 * x4 + x86);
    diag[24] += x43 * (x35 + x38 * x4 + x87);
    diag[25] += x43 * (x4 * x48 + x46 + x88);
    diag[26] += x43 * (x4 * x57 + x55 + x89);
    diag[27] += x43 * (x4 * x68 + x65 + x90);
    diag[28] += x43 * (x4 * x75 + x74 + x91);
    diag[29] += x43 * (x4 * x80 + x79 + x92);
}

static const int n_qp = 8;
static const scalar_t qx[8] =
    {0.0, 1.0, 0.0, 0.0, 0.333333333333, 0.333333333333, 0.0, 0.333333333333};

static const scalar_t qy[8] =
    {0.0, 0.0, 1.0, 0.0, 0.333333333333, 0.0, 0.333333333333, 0.333333333333};

static const scalar_t qz[8] =
    {0.0, 0.0, 0.0, 1.0, 0.0, 0.333333333333, 0.333333333333, 0.333333333333};

static const scalar_t qw[8] = {0.025, 0.025, 0.025, 0.025, 0.225, 0.225, 0.225, 0.225};

void tet10_linear_elasticity_init(linear_elasticity_t *const ctx,
                                  const real_t mu,
                                  const real_t lambda,
                                  const ptrdiff_t nelements,
                                  idx_t **const SFEM_RESTRICT elements,
                                  geom_t **const SFEM_RESTRICT points) {
    jacobian_t *jacobian_adjugate = (jacobian_t *)calloc(9 * nelements, sizeof(jacobian_t));
    jacobian_t *jacobian_determinant = (jacobian_t *)calloc(nelements, sizeof(jacobian_t));

#pragma omp parallel
    {
#pragma omp for
        for (ptrdiff_t e = 0; e < nelements; e++) {
            adjugate_and_det_micro_kernel(points[0][elements[0][e]],
                                          points[0][elements[1][e]],
                                          points[0][elements[2][e]],
                                          points[0][elements[3][e]],
                                          points[1][elements[0][e]],
                                          points[1][elements[1][e]],
                                          points[1][elements[2][e]],
                                          points[1][elements[3][e]],
                                          points[2][elements[0][e]],
                                          points[2][elements[1][e]],
                                          points[2][elements[2][e]],
                                          points[2][elements[3][e]],
                                          &jacobian_adjugate[e * 9],
                                          &jacobian_determinant[e]);
        }
    }

    ctx->mu = mu;
    ctx->lambda = lambda;
    ctx->jacobian_adjugate = jacobian_adjugate;
    ctx->jacobian_determinant = jacobian_determinant;
    ctx->elements = elements;
    ctx->nelements = nelements;
    ctx->element_type = TET10;
}

void tet10_linear_elasticity_destroy(linear_elasticity_t *const ctx) {
    free(ctx->jacobian_adjugate);
    free(ctx->jacobian_determinant);

    ctx->jacobian_adjugate = 0;
    ctx->jacobian_determinant = 0;

    ctx->elements = 0;
    ctx->nelements = 0;
    ctx->element_type = INVALID;
}

void tet10_linear_elasticity_apply_opt(const linear_elasticity_t *const ctx,
                                       const real_t *const SFEM_RESTRICT u,
                                       real_t *const SFEM_RESTRICT values) {
    const scalar_t mu = ctx->mu;
    const scalar_t lambda = ctx->lambda;

    const jacobian_t *const g_jacobian_adjugate = (jacobian_t *)ctx->jacobian_adjugate;
    const jacobian_t *const g_jacobian_determinant = (jacobian_t *)ctx->jacobian_determinant;

#pragma omp parallel
    {
#pragma omp for  // nowait
        for (ptrdiff_t i = 0; i < ctx->nelements; ++i) {
            idx_t ev[10];

            scalar_t element_u[30] = {0};
            accumulator_t element_vector[30] = {0};

            const jacobian_t *const jacobian_adjugate = &g_jacobian_adjugate[i * 9];
            const jacobian_t jacobian_determinant = g_jacobian_determinant[i];

#pragma unroll(10)
            for (int v = 0; v < 10; ++v) {
                ev[v] = ctx->elements[v][i];
            }

            for (int v = 0; v < 10; ++v) {
                element_u[v] = u[ev[v] * 3];
                element_u[10 + v] = u[ev[v] * 3 + 1];
                element_u[20 + v] = u[ev[v] * 3 + 2];
            }

            for (int k = 0; k < n_qp; k++) {
                tet10_linear_elasticity_apply_micro_kernel(mu,
                                                           lambda,
                                                           jacobian_adjugate,
                                                           jacobian_determinant,
                                                           qx[k],
                                                           qy[k],
                                                           qz[k],
                                                           qw[k],
                                                           element_u,
                                                           element_vector);
            }

#pragma unroll(10)
            for (int v = 0; v < 10; v++) {
#pragma omp atomic update
                values[ev[v] * 3] += element_vector[v];
            }

#pragma unroll(10)
            for (int v = 0; v < 10; v++) {
#pragma omp atomic update
                values[ev[v] * 3 + 1] += element_vector[10 + v];
            }

#pragma unroll(10)
            for (int v = 0; v < 10; v++) {
#pragma omp atomic update
                values[ev[v] * 3 + 2] += element_vector[20 + v];
            }
        }
    }
}

void tet10_linear_elasticity_diag_opt(const linear_elasticity_t *const ctx,
                                      real_t *const SFEM_RESTRICT values) {
    const scalar_t mu = ctx->mu;
    const scalar_t lambda = ctx->lambda;

    const jacobian_t *const g_jacobian_adjugate = (jacobian_t *)ctx->jacobian_adjugate;
    const jacobian_t *const g_jacobian_determinant = (jacobian_t *)ctx->jacobian_determinant;

#pragma omp parallel
    {
#pragma omp for  // nowait
        for (ptrdiff_t i = 0; i < ctx->nelements; ++i) {
            idx_t ev[10];
            scalar_t element_vector[30] = {0};

            const jacobian_t *const jacobian_adjugate = &g_jacobian_adjugate[i * 9];
            const jacobian_t jacobian_determinant = g_jacobian_determinant[i];

#pragma unroll(10)
            for (int v = 0; v < 10; ++v) {
                ev[v] = ctx->elements[v][i];
            }

            for (int k = 0; k < n_qp; k++) {
                diag_micro_kernel(mu,
                                  lambda,
                                  jacobian_adjugate,
                                  jacobian_determinant,
                                  qx[k],
                                  qy[k],
                                  qz[k],
                                  qw[k],
                                  element_vector);
            }

#pragma unroll(10)
            for (int v = 0; v < 10; v++) {
#pragma omp atomic update
                values[ev[v] * 3] += element_vector[v];
            }

#pragma unroll(10)
            for (int v = 0; v < 10; v++) {
#pragma omp atomic update
                values[ev[v] * 3 + 1] += element_vector[10 + v];
            }

#pragma unroll(10)
            for (int v = 0; v < 10; v++) {
#pragma omp atomic update
                values[ev[v] * 3 + 2] += element_vector[20 + v];
            }
        }
    }
}

void tet10_linear_elasticity_apply_aos(const ptrdiff_t nelements,
                                       const ptrdiff_t nnodes,
                                       idx_t **const SFEM_RESTRICT elements,
                                       geom_t **const SFEM_RESTRICT points,
                                       const real_t mu,
                                       const real_t lambda,
                                       const real_t *const SFEM_RESTRICT u,
                                       real_t *const SFEM_RESTRICT values) {
#if 0
    linear_elasticity_t ctx;
    tet10_linear_elasticity_init(&ctx, mu, lambda, nelements, elements, points);
    tet10_linear_elasticity_apply_opt(&ctx, u, values);
    tet10_linear_elasticity_destroy(&ctx);
#else
    static linear_elasticity_t ctx;
    static int initialized = 0;

    if (!initialized) {
        tet10_linear_elasticity_init(&ctx, mu, lambda, nelements, elements, points);
        initialized = 1;
    }

    tet10_linear_elasticity_apply_opt(&ctx, u, values);
#endif
}

void tet10_linear_elasticity_assemble_diag_aos(const ptrdiff_t nelements,
                                               const ptrdiff_t nnodes,
                                               idx_t **const SFEM_RESTRICT elements,
                                               geom_t **const SFEM_RESTRICT points,
                                               const real_t mu,
                                               const real_t lambda,
                                               real_t *const SFEM_RESTRICT values) {
#if 0
    linear_elasticity_t ctx;
    tet10_linear_elasticity_init(&ctx, mu, lambda, nelements, elements, points);
    tet10_linear_elasticity_apply_opt(&ctx, u, values);
    tet10_linear_elasticity_destroy(&ctx);
#else
    static linear_elasticity_t ctx;
    static int initialized = 0;

    if (!initialized) {
        tet10_linear_elasticity_init(&ctx, mu, lambda, nelements, elements, points);
        initialized = 1;
    }

    tet10_linear_elasticity_diag_opt(&ctx, values);
#endif
}
