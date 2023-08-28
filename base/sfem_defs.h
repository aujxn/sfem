#ifndef SFEM_DEFS_H
#define SFEM_DEFS_H

#include "sfem_base.h"

#include <assert.h>

enum ElemType { NODE1 = 1, EDGE2=2, TRI3 = 3, TET4 = 4, TRI6 = 6, HEX8 = 8, TET10 = 10, EDGE3=11, INVALID = -1 };

SFEM_INLINE static enum ElemType side_type(const enum ElemType type) {
    switch (type) {
        case TRI3:
            return EDGE2;
        case TRI6:
            return EDGE3;
        case TET4:
            return TRI3;
        case TET10:
            return TRI6;
        case EDGE2:
            return NODE1;
        default: {
            assert(0);
            return INVALID;
        }
    }
}

SFEM_INLINE static enum ElemType elem_lower_order(const enum ElemType type) {
    switch (type) {
        case TRI6:
            return TRI3;
        case TET10:
            return TET4;
        case EDGE3:
            return EDGE2;
        default: {
            assert(0);
            return INVALID;
        }
    }
}

SFEM_INLINE static int elem_num_nodes(const enum ElemType type) {
    switch (type) {
        case NODE1:
            return 1;
        case EDGE2:
            return 2;
        case EDGE3:
            return 3;
        case TRI3:
            return 3;
        case TET4:
            return 4;
        case TRI6:
            return 6;
        case HEX8:
            return 8;
        case TET10:
            return 10;
        default: {
            assert(0);
            return 0;
        }
    }
}

SFEM_INLINE static int elem_num_sides(const enum ElemType type) {
    switch (type) {
        case EDGE2:
            return 2;
        case TRI3:
            return 3;
        case TET4:
            return 4;
        case TRI6:
            return 3;
        case HEX8:
            return 6;
        case TET10:
            return 4;
        default: {
            assert(0);
            return 0;
        }
    }
}

SFEM_INLINE static int elem_manifold_dim(const enum ElemType type) {
    switch (type) {
        case EDGE2:
            return 1;
        case TRI3:
            return 2;
        case TET4:
            return 3;
        case TRI6:
            return 2;
        case HEX8:
            return 3;
        case TET10:
            return 3;
        default: {
            assert(0);
            return INVALID;
        }
    }
}


#endif  // SFEM_DEFS_H
