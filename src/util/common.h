/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__common__h_
#define util__common__h_

#define x_JOIN(a, b) x_JOIN_(a, b)
#define x_JOIN_(a, b) a##b

#define x_ARGS_NUM(...) x_ARGS_NUM_(__VA_ARGS__, x_RSEQ_)
#define x_ARGS_NUM_(...) x_ARG_N_(__VA_ARGS__)
#define x_RSEQ_ 8, 7, 6, 5, 4, 3, 2, 1
#define x_ARG_N_(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N

#define x_ARG(N, ...) x_ARG_##N##_(__VA_ARGS__)
#define x_ARG_1_(_1, ...) _1
#define x_ARG_2_(_1, _2, ...) _2
#define x_ARG_3_(_1, _2, _3, ...) _3
#define x_ARG_4_(_1, _2, _3, _4, ...) _4
#define x_ARG_5_(_1, _2, _3, _4, _5, ...) _5
#define x_ARG_6_(_1, _2, _3, _4, _5, _6, ...) _6
#define x_ARG_7_(_1, _2, _3, _4, _5, _6, _7, ...) _7
#define x_ARG_8_(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8

#define x_default_clone(v) (v)
#define x_default_drop(vp) ((void)(vp))

#define x_make_array(T, ...) ((T [])__VA_ARGS__)
#define x_literal_count(T, ...) (sizeof((T [])__VA_ARGS__) / sizeof(T))

#define x_make(C, ...)                        \
    C##_from_n(                               \
        x_make_array(C##_raw, __VA_ARGS__),   \
        x_literal_count(C##_raw, __VA_ARGS__) \
    )

#endif // util__common__h_
