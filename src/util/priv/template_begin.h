/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__priv__template_begin__h_
#define util__priv__template_begin__h_

#include "common.h"

#define x_DEFINE_TYPE_(MACRO, SELF, ...) MACRO(SELF, __VA_ARGS__)
#define x_MEMB_(name) x_JOIN(t_self, x_JOIN(_, name))
#define x_m_value_ x_MEMB_(value)
#define x_m_raw_ x_MEMB_(raw)

#endif // util__priv__template_begin__h_

#if defined T && x_ARGS_NUM(T) > 1
  #define t_self x_ARG(1, T)
  #define t_value x_ARG(2, T)
#else
  #ifndef t_tag
    #error "No t_tag defined"
  #endif

  #define t_self x_JOIN(t_prefix_, x_JOIN(_, t_tag))
  #ifdef T
    #define t_value T
  #endif
#endif

#ifndef t_value
  #error "No t_value defined"
#endif

#ifndef t_raw
  #define t_raw t_value
#endif

#ifndef t_value_from
  #define t_value_from(v) x_default_clone(v)
#endif

#ifndef t_value_clone
  #define t_value_clone(v) x_default_clone(v)
#endif

#ifndef t_value_drop
  #define t_value_drop(vp) x_default_drop(vp)
#endif
