/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef template__begin__h_
#define template__begin__h_

#include "preproc.h"

#define DEFINE_TYPE(MACRO, SELF, ...) EXPAND(MACRO(SELF, __VA_ARGS__))
#define MEMB(name) JOIN(Self, name)
#define SelfValue MEMB(_value_type)

#endif // template__begin__h_

#ifndef T
  #error "T is undefined"
#elif ARGS_NUM(T) != 2
  #error "T must have exactly two arguments"
#endif

#define Self ARG(1, T)
#define ValueType ARG(2, T)

#ifndef value_free
  #define value_free(vp) ((void)(vp))
#endif
