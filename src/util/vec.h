/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef util__vec__h_
#define util__vec__h_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define x_VEC_SHRUNK_SIZE_ ((size_t)-1)

#endif // util__vec__h_

#ifndef t_prefix_
  #define t_prefix_ vec
#endif
#include "priv/template_begin.h"

#ifndef DECLARED
  x_DEFINE_TYPE_(x_declare_vec, t_self, t_value);
#endif
#undef DECLARED

typedef t_raw x_m_raw_;

static inline t_self x_MEMB_(new)(void) {
    return (t_self){0};
}

static inline t_self x_MEMB_(with_capacity)(size_t cap) {
    x_m_value_ *d = malloc(cap * sizeof(x_m_value_));
    return (d) ? (t_self){d, 0, cap} : (t_self){0};
}

static inline t_self x_MEMB_(move)(t_self *self) {
    t_self m = *self;
    memset(self, 0, sizeof(t_self));
    return m;
}

static inline size_t x_MEMB_(size)(const t_self *self) {
    return self->size;
}

static inline size_t x_MEMB_(capacity)(const t_self *self) {
    return self->capacity;
}

static inline bool x_MEMB_(is_empty)(const t_self *self) {
    return self->size == 0;
}

static inline void x_MEMB_(clear)(t_self *self) {
    for (size_t i = 0; i < self->size; i++) {
        t_value_drop(self->data + i);
    }
    self->size = 0;
}

static inline void x_MEMB_(drop)(t_self *self) {
    x_MEMB_(clear)(self);
    free(self->data);

    self->capacity = 0;
    self->data = NULL;
}

static inline void x_MEMB_(take)(t_self *self, t_self unowned) {
    x_MEMB_(drop)(self);
    *self = unowned;
}

static inline bool x_MEMB_(reserve)(t_self *self, size_t cap) {
    if (cap <= self->capacity) return true;

    size_t n = (cap == x_VEC_SHRUNK_SIZE_) ? self->size : cap;

    x_m_value_ *d;
    if (n == 0) {
        d = NULL;
        free(self->data);
    } else {
        d = (x_m_value_ *)realloc(self->data, n * sizeof(*self->data));
        if (!d) return false;
    }

    self->data = d;
    self->capacity = n;
    return true;
}

static inline t_self x_MEMB_(clone)(const t_self *self) {
    t_self out = x_MEMB_(with_capacity)(self->size);
    if (out.data) {
        out.size = out.capacity;
        for (size_t i = 0; i < out.size; i++) {
            out.data[i] = t_value_clone(self->data[i]);
        }
    }
    return out;
}

static inline bool x_MEMB_(copy)(t_self *self, const t_self *other) {
    x_MEMB_(clear)(self);
    if (!x_MEMB_(reserve)(self, other->size)) return false;

    self->size = self->capacity;
    for (size_t i = 0; i < self->size; i++) {
        self->data[i] = t_value_clone(other->data[i]);
    }
    return true;
}

static inline x_m_value_ *x_MEMB_(push)(t_self *self, x_m_value_ val) {
    if (self->size == self->capacity) {
        size_t n = (self->capacity == 0) ? 8 : self->capacity * 2;

        if (!x_MEMB_(reserve)(self, n)) return NULL;
    }

    x_m_value_ *v = self->data + self->size++;
    *v = val;

    return v;
}

static inline x_m_value_ *x_MEMB_(emplace)(t_self *self, x_m_raw_ raw) {
    return x_MEMB_(push)(self, t_value_from(raw));
}

static inline t_self x_MEMB_(from_n)(const x_m_raw_ *arr, size_t n) {
    t_self out = x_MEMB_(with_capacity)(n);
    if (out.data) {
        for (size_t i = 0; i < n; i++) {
            x_MEMB_(push)(&out, t_value_from(arr[i]));
        }
        out.size = n;
    }
    return out;
}

static inline void x_MEMB_(pop)(t_self *self) {
    x_m_value_ *v = &self->data[--self->size];
    t_value_drop(v);
}

static inline void x_MEMB_(erase_n)(t_self *self, size_t idx, size_t len) {
    x_m_value_ *d = self->data + idx;
    x_m_value_ *p = d;
    x_m_value_ *end = self->data + self->size;

    for (size_t i = 0; i < len; i++) {
        t_value_drop(p++);
    }

    memmove(d, p, (end - p) * sizeof(*self->data));
    self->size -= len;
}

static inline void x_MEMB_(shrink_to_fit)(t_self *self) {
    x_MEMB_(reserve)(self, x_VEC_SHRUNK_SIZE_);
}

static inline bool x_MEMB_(insert_uninit_)(t_self *self, size_t idx, size_t n) {
    if (self->size + n > self->capacity) {
        size_t standard_cap = (self->capacity == 0) ? 8 : self->capacity * 2;
        size_t min_req_cap = self->size + n;
        size_t cap = (standard_cap > min_req_cap) ? standard_cap : min_req_cap;

        if (!x_MEMB_(reserve)(self, cap)) return false;
    }

    x_m_value_ *pos = self->data + idx;
    memmove(pos + n, pos, (self->size - idx) * sizeof(*pos));

    self->size += n;

    return true;
}

static inline bool x_MEMB_(insert_n)(t_self *self, size_t idx, const x_m_value_ *arr, size_t n) {
    if (!x_MEMB_(insert_uninit_)(self, idx, n)) return false;

    memcpy(self->data + idx, arr, n * sizeof(x_m_value_));
    return true;
}

static inline bool x_MEMB_(emplace_n)(t_self *self, size_t idx, const x_m_raw_ *arr, size_t n) {
    if (!x_MEMB_(insert_uninit_)(self, idx, n)) return false;

    for (x_m_value_ *p = self->data + idx; n--; p++, arr++) {
        *p = t_value_from(*arr);
    }
    return true;
}

static inline const x_m_value_ *x_MEMB_(at)(const t_self *self, size_t idx) {
    return self->data + idx;
}

static inline x_m_value_ *x_MEMB_(at_mut)(t_self *self, size_t idx) {
    return self->data + idx;
}

#include "priv/template_end.h"
