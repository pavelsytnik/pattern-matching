/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__begin__h_
#define util__begin__h_

#include "common.h"

#define x_DEFINE_TYPE(MACRO, SELF, ...) MACRO(SELF, __VA_ARGS__)
#define x_MEMB(name) x_JOIN(Self, x_JOIN(_, name))
#define SelfValue x_MEMB(value)
#define SelfRaw x_MEMB(raw)

#endif // util__begin__h_

#ifndef T
  #error "T is undefined"
#elif x_ARGS_NUM(T) != 2
  #error "T must have exactly two arguments"
#endif

#define Self x_ARG(1, T)
#define TValue x_ARG(2, T)

#ifndef TRaw
  #define TRaw TValue
#endif

#ifndef t_value_from
  #define t_value_from(v) (v)
#endif

#ifndef t_value_clone
  #define t_value_clone(v) (v)
#endif

#ifndef t_value_drop
  #define t_value_drop(vp) ((void)(vp))
#endif
