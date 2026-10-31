/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__types__h_
#define util__types__h_

#define DECLARE_VEC(SELF, TYPE) \
    typedef TYPE SELF##_value;  \
    typedef struct SELF {       \
        SELF##_value *data;     \
        size_t size;            \
        size_t capacity;        \
    } SELF

#endif // util__types__h_
