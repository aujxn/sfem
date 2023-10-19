#ifndef SFEM_RESAMPLE_GAP_H
#define SFEM_RESAMPLE_GAP_H

#include <stddef.h>
#include <mpi.h>

#include "sfem_base.h"
#include "sfem_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

int resample_gap(
                 // Mesh
                 const enum ElemType element_type,
                 const ptrdiff_t nelements,
                 const ptrdiff_t nnodes,
                 idx_t **const SFEM_RESTRICT elems,
                 geom_t **const SFEM_RESTRICT xyz,
                 // SDF
                 const ptrdiff_t *const SFEM_RESTRICT n,
                 const ptrdiff_t *const SFEM_RESTRICT stride,
                 const geom_t *const SFEM_RESTRICT origin,
                 const geom_t *const SFEM_RESTRICT delta,
                 const geom_t *const SFEM_RESTRICT data,
                 // Output
                 real_t *const SFEM_RESTRICT g,
                 real_t *const SFEM_RESTRICT xnormal,
                 real_t *const SFEM_RESTRICT ynormal,
                 real_t *const SFEM_RESTRICT znormal);

int interpolate_gap(const ptrdiff_t nnodes,
                    geom_t **const SFEM_RESTRICT xyz,
                    // SDF
                    const ptrdiff_t *const SFEM_RESTRICT n,
                    const ptrdiff_t *const SFEM_RESTRICT stride,
                    const geom_t *const SFEM_RESTRICT origin,
                    const geom_t *const SFEM_RESTRICT delta,
                    const geom_t *const SFEM_RESTRICT data,
                    // Output
                    real_t *const SFEM_RESTRICT g,
                    real_t *const SFEM_RESTRICT xnormal,
                    real_t *const SFEM_RESTRICT ynormal,
                    real_t *const SFEM_RESTRICT znormal);

#ifdef __cplusplus
}
#endif

#endif // SFEM_RESAMPLE_GAP_H
