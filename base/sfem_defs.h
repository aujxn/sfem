#ifndef SFEM_DEFS_H
#define SFEM_DEFS_H

#include "sfem_base.h"

#include <assert.h>

enum ElemType { TRI3 = 3, TET4 = 4, TRI6 = 6, HEX8 = 8, TET10 = 10, INVALID = -1 };

SFEM_INLINE static enum ElemType side_type(const enum ElemType type) {
    switch (type) {
        case TET4:
            return TRI3;
        case TET10:
            return TRI6;
        default: {
            assert(false);
            return INVALID;
        }
    }
}

SFEM_INLINE static enum ElemType elem_num_nodes(const enum ElemType type) {
    switch (type) {
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
            assert(false);
            return INVALID;
        }
    }
}

SFEM_INLINE static int elem_num_sides(const enum ElemType type) {
    switch (type) {
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
            assert(false);
            return INVALID;
        }
    }
}


#endif  // SFEM_DEFS_H
