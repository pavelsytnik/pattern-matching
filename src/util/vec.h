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

#include "begin.h"

#ifndef DECLARED
  x_DEFINE_TYPE(x_DECLARE_VEC, Self, TValue);
#endif
#undef DECLARED

typedef TRaw SelfRaw;

static inline Self x_MEMB(new)(void) {
    return (Self){0};
}

static inline Self x_MEMB(with_capacity)(size_t cap) {
    SelfValue *d = malloc(cap * sizeof(SelfValue));
    return (d) ? (Self){d, 0, cap} : (Self){0};
}

static inline Self x_MEMB(move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof(Self));
    return m;
}

static inline size_t x_MEMB(size)(const Self *self) {
    return self->size;
}

static inline size_t x_MEMB(capacity)(const Self *self) {
    return self->capacity;
}

static inline bool x_MEMB(is_empty)(const Self *self) {
    return self->size == 0;
}

static inline void x_MEMB(clear)(Self *self) {
    for (size_t i = 0; i < self->size; i++) {
        t_value_drop(self->data + i);
    }
    self->size = 0;
}

static inline void x_MEMB(drop)(Self *self) {
    x_MEMB(clear)(self);
    free(self->data);

    self->capacity = 0;
    self->data = NULL;
}

static inline void x_MEMB(take)(Self *self, Self unowned) {
    x_MEMB(drop)(self);
    *self = unowned;
}

static inline bool x_MEMB(reserve)(Self *self, size_t cap) {
    if (cap <= self->capacity) return true;

    size_t n = (cap == x_VEC_SHRUNK_SIZE_) ? self->size : cap;

    SelfValue *d;
    if (n == 0) {
        d = NULL;
        free(self->data);
    } else {
        d = (SelfValue *)realloc(self->data, n * sizeof(*self->data));
        if (!d) return false;
    }

    self->data = d;
    self->capacity = n;
    return true;
}

static inline Self x_MEMB(clone)(const Self *self) {
    Self out = x_MEMB(with_capacity)(self->size);
    if (out.data) {
        out.size = out.capacity;
        for (size_t i = 0; i < out.size; i++) {
            out.data[i] = t_value_clone(self->data[i]);
        }
    }
    return out;
}

static inline bool x_MEMB(copy)(Self *self, const Self *other) {
    x_MEMB(clear)(self);
    if (!x_MEMB(reserve)(self, other->size)) return false;

    self->size = self->capacity;
    for (size_t i = 0; i < self->size; i++) {
        self->data[i] = t_value_clone(other->data[i]);
    }
    return true;
}

static inline SelfValue *x_MEMB(push)(Self *self, SelfValue val) {
    if (self->size == self->capacity) {
        size_t n = (self->capacity == 0) ? 8 : self->capacity * 2;

        if (!x_MEMB(reserve)(self, n)) return NULL;
    }

    SelfValue *v = self->data + self->size++;
    *v = val;

    return v;
}

static inline SelfValue *x_MEMB(emplace)(Self *self, SelfRaw raw) {
    return x_MEMB(push)(self, t_value_from(raw));
}

static inline Self x_MEMB(from_n)(const SelfRaw *arr, size_t n) {
    Self out = x_MEMB(with_capacity)(n);
    if (out.data) {
        for (size_t i = 0; i < n; i++) {
            x_MEMB(push)(&out, t_value_from(arr[i]));
        }
        out.size = n;
    }
    return out;
}

static inline void x_MEMB(pop)(Self *self) {
    SelfValue *v = &self->data[--self->size];
    t_value_drop(v);
}

static inline void x_MEMB(erase_n)(Self *self, size_t idx, size_t len) {
    SelfValue *d = self->data + idx;
    SelfValue *p = d;
    SelfValue *end = self->data + self->size;

    for (size_t i = 0; i < len; i++) {
        t_value_drop(p++);
    }

    memmove(d, p, (end - p) * sizeof(*self->data));
    self->size -= len;
}

static inline void x_MEMB(shrink_to_fit)(Self *self) {
    x_MEMB(reserve)(self, x_VEC_SHRUNK_SIZE_);
}

static inline bool x_MEMB(insert_uninit_)(Self *self, size_t idx, size_t n) {
    if (self->size + n > self->capacity) {
        size_t standard_cap = (self->capacity == 0) ? 8 : self->capacity * 2;
        size_t min_req_cap = self->size + n;
        size_t cap = (standard_cap > min_req_cap) ? standard_cap : min_req_cap;

        if (!x_MEMB(reserve)(self, cap)) return false;
    }

    SelfValue *pos = self->data + idx;
    memmove(pos + n, pos, (self->size - idx) * sizeof(*pos));

    self->size += n;

    return true;
}

static inline bool x_MEMB(insert_n)(Self *self, size_t idx, const SelfValue *arr, size_t n) {
    if (!x_MEMB(insert_uninit_)(self, idx, n)) return false;

    memcpy(self->data + idx, arr, n * sizeof(SelfValue));
    return true;
}

static inline bool x_MEMB(emplace_n)(Self *self, size_t idx, const SelfRaw *arr, size_t n) {
    if (!x_MEMB(insert_uninit_)(self, idx, n)) return false;

    for (SelfValue *p = self->data + idx; n--; p++, arr++) {
        *p = t_value_from(*arr);
    }
    return true;
}

static inline const SelfValue *x_MEMB(at)(const Self *self, size_t idx) {
    return self->data + idx;
}

static inline SelfValue *x_MEMB(at_mut)(Self *self, size_t idx) {
    return self->data + idx;
}

#include "end.h"
