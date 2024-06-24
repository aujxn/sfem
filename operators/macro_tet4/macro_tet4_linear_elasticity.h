#ifndef MACRO_TET4_LINEAR_ELASTICITY_H
#define MACRO_TET4_LINEAR_ELASTICITY_H

#include <stddef.h>
#include "sfem_base.h"
#include "sfem_defs.h"
#include "tet4_linear_elasticity.h"

#ifdef __cplusplus
extern "C" {
#endif

int macro_tet4_linear_elasticity_apply_opt(const ptrdiff_t nelements,
                                           const jacobian_t *const SFEM_RESTRICT jacobian_adjugate,
                                           const jacobian_t *const SFEM_RESTRICT
                                                   jacobian_determinant,
                                           const ptrdiff_t u_stride,
                                           const real_t *const SFEM_RESTRICT ux,
                                           const real_t *const SFEM_RESTRICT uy,
                                           const real_t *const SFEM_RESTRICT uz,
                                           const ptrdiff_t out_stride,
                                           const real_t *const SFEM_RESTRICT outx,
                                           const real_t *const SFEM_RESTRICT outy,
                                           const real_t *const SFEM_RESTRICT outz);

int macro_tet4_linear_elasticity_diag(const ptrdiff_t nelements,
                                      const jacobian_t *const SFEM_RESTRICT jacobian_adjugate,
                                      const jacobian_t *const SFEM_RESTRICT jacobian_determinant,
                                      const ptrdiff_t out_stride,
                                      const real_t *const SFEM_RESTRICT outx,
                                      const real_t *const SFEM_RESTRICT outy,
                                      const real_t *const SFEM_RESTRICT outz);

int macro_tet4_linear_elasticity_apply(const ptrdiff_t nelements,
                                       const ptrdiff_t nnodes,
                                       idx_t **const SFEM_RESTRICT elements,
                                       geom_t **const SFEM_RESTRICT points,
                                       const real_t mu,
                                       const real_t lambda,
                                       const ptrdiff_t u_stride,
                                       const real_t *const SFEM_RESTRICT ux,
                                       const real_t *const SFEM_RESTRICT uy,
                                       const real_t *const SFEM_RESTRICT uz,
                                       const ptrdiff_t out_stride,
                                       const real_t *const SFEM_RESTRICT outx,
                                       const real_t *const SFEM_RESTRICT outy,
                                       const real_t *const SFEM_RESTRICT outz);

#ifdef __cplusplus
}
#endif
#endif  // MACRO_TET4_LINEAR_ELASTICITY_H
