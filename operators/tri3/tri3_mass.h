#ifndef TRI3_MASS_H
#define TRI3_MASS_H
#include <stddef.h>
#include "sfem_base.h"
#include "sfem_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

void tri3_apply_mass(const ptrdiff_t nelements,
                     const ptrdiff_t nnodes,
                     idx_t **const SFEM_RESTRICT elems,
                     geom_t **const SFEM_RESTRICT xyz,
                     const real_t *const x,
                     real_t *const values);

void tri3_apply_inv_lumped_mass(const ptrdiff_t nelements,
                                const ptrdiff_t nnodes,
                                idx_t **const SFEM_RESTRICT elems,
                                geom_t **const SFEM_RESTRICT points,
                                const real_t *const x,
                                real_t *const values);

void tri3_assemble_mass(const ptrdiff_t nelements,
                        const ptrdiff_t nnodes,
                        idx_t **const SFEM_RESTRICT elems,
                        geom_t **const SFEM_RESTRICT xyz,
                        count_t *const SFEM_RESTRICT rowptr,
                        idx_t *const SFEM_RESTRICT colidx,
                        real_t *const SFEM_RESTRICT values);

#ifdef __cplusplus
}
#endif

#endif  // TRI3_MASS_H
