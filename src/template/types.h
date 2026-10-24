/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef template__types__h_
#define template__types__h_

#define DECLARE_VEC(SELF, TYPE)     \
    typedef TYPE SELF##_value_type; \
    typedef struct SELF {           \
        SELF##_value_type *data;    \
        size_t size;                \
        size_t capacity;            \
    } SELF

#endif // template__types__h_
