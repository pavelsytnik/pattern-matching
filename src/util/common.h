/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__common__h_
#define util__common__h_

#define EXPAND(X) X

#define JOIN(a, b) JOIN_(a, b)
#define JOIN_(a, b) a##b

#define ARGS_NUM(...) ARGS_NUM_(__VA_ARGS__, RSEQ_)
#define ARGS_NUM_(...) EXPAND(ARG_N_(__VA_ARGS__))
#define RSEQ_ 8, 7, 6, 5, 4, 3, 2, 1
#define ARG_N_(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N

#define ARG(N, ...) EXPAND(ARG_##N##_(__VA_ARGS__))
#define ARG_1_(_1, ...) _1
#define ARG_2_(_1, _2, ...) _2
#define ARG_3_(_1, _2, _3, ...) _3
#define ARG_4_(_1, _2, _3, _4, ...) _4
#define ARG_5_(_1, _2, _3, _4, _5, ...) _5
#define ARG_6_(_1, _2, _3, _4, _5, _6, ...) _6
#define ARG_7_(_1, _2, _3, _4, _5, _6, _7, ...) _7
#define ARG_8_(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8

#define MAKE_ARRAY(T, ...) ((T [])__VA_ARGS__)
#define LITERAL_COUNT(T, ...) (sizeof((T [])__VA_ARGS__) / sizeof(T))

#define MAKE(C, ...)                        \
    C##_from_n(                             \
        MAKE_ARRAY(C##_raw, __VA_ARGS__),   \
        LITERAL_COUNT(C##_raw, __VA_ARGS__) \
    )

#endif // util__common__h_
