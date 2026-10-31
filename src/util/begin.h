/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__begin__h_
#define util__begin__h_

#include "common.h"

#define DEFINE_TYPE(MACRO, SELF, ...) EXPAND(MACRO(SELF, __VA_ARGS__))
#define MEMB(name) JOIN(Self, name)
#define SelfValue MEMB(_value_type)
#define SelfRaw MEMB(_raw_type)

#endif // util__begin__h_

#ifndef T
  #error "T is undefined"
#elif ARGS_NUM(T) != 2
  #error "T must have exactly two arguments"
#endif

#define Self ARG(1, T)
#define ValueType ARG(2, T)

#ifndef RawValueType
  #define RawValueType ValueType
#endif

#ifndef value_from
  #define value_from(v) (v)
#endif

#ifndef value_clone
  #define value_clone(v) (v)
#endif

#ifndef value_free
  #define value_free(vp) ((void)(vp))
#endif
