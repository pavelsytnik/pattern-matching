/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef template__types__h_
#define template__types__h_

#define MAKE_ARRAY(T, ...) ((T [])__VA_ARGS__)
#define LITERAL_COUNT(T, ...) (sizeof((T [])__VA_ARGS__) / sizeof(T))

#define MAKE(C, ...)                               \
    C##_from(                                      \
        MAKE_ARRAY(C##_value_type, __VA_ARGS__),   \
        LITERAL_COUNT(C##_value_type, __VA_ARGS__) \
    )

#define DECLARE_VEC(SELF, TYPE)     \
    typedef TYPE SELF##_value_type; \
    typedef struct SELF {           \
        SELF##_value_type *data;    \
        size_t size;                \
        size_t capacity;            \
    } SELF

#endif // template__types__h_
