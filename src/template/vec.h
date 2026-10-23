/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef template__vec__h_
#define template__vec__h_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define VEC_SHRUNK_SIZE_ ((size_t)-1)

#endif // template__vec__h_

#include "begin.h"

#ifndef DECLARED
  DEFINE_TYPE(DECLARE_VEC, Self, ValueType);
#endif
#undef DECLARED

typedef RawValueType SelfRaw;

static inline Self MEMB(_init)(void) {
    return (Self){0};
}

static inline Self MEMB(_with_capacity)(size_t cap) {
    SelfValue *d = malloc(cap * sizeof(SelfValue));
    return (d) ? (Self){d, 0, cap} : (Self){0};
}

static inline Self MEMB(_move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof(Self));
    return m;
}

static inline size_t MEMB(_size)(const Self *self) {
    return self->size;
}

static inline size_t MEMB(_capacity)(const Self *self) {
    return self->capacity;
}

static inline bool MEMB(_is_empty)(const Self *self) {
    return self->size == 0;
}

static inline void MEMB(_clear)(Self *self) {
    for (size_t i = 0; i < self->size; i++) {
        value_free(self->data + i);
    }
    self->size = 0;
}

static inline void MEMB(_drop)(Self *self) {
    MEMB(_clear)(self);
    free(self->data);

    self->capacity = 0;
    self->data = NULL;
}

static inline void MEMB(_take)(Self *self, Self unowned) {
    MEMB(_drop)(self);
    *self = unowned;
}

static inline bool MEMB(_reserve)(Self *self, size_t cap) {
    if (cap <= self->capacity) return true;

    size_t n = (cap == VEC_SHRUNK_SIZE_) ? self->size : cap;

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

static inline SelfValue *MEMB(_push)(Self *self, SelfValue val) {
    if (self->size == self->capacity) {
        size_t n = (self->capacity == 0) ? 8 : self->capacity * 2;

        if (!MEMB(_reserve)(self, n)) return NULL;
    }

    SelfValue *v = self->data + self->size++;
    *v = val;

    return v;
}

static inline SelfValue *MEMB(_emplace)(Self *self, SelfRaw raw) {
    return MEMB(_push)(self, value_from(raw));
}

static inline Self MEMB(_from)(const SelfRaw *arr, size_t n) {
    Self out = MEMB(_with_capacity)(n);
    if (out.data) {
        for (size_t i = 0; i < n; i++) {
            MEMB(_push)(&out, value_from(arr[i]));
        }
        out.size = n;
    }
    return out;
}

static inline void MEMB(_pop)(Self *self) {
    SelfValue *v = &self->data[--self->size];
    value_free(v);
}

static inline void MEMB(_erase)(Self *self, size_t idx, size_t len) {
    SelfValue *d = self->data + idx;
    SelfValue *p = d;
    SelfValue *end = self->data + self->size;

    for (size_t i = 0; i < len; i++) {
        value_free(p++);
    }

    memmove(d, p, (end - p) * sizeof(*self->data));
    self->size -= len;
}

static inline void MEMB(_shrink_to_fit)(Self *self) {
    MEMB(_reserve)(self, VEC_SHRUNK_SIZE_);
}

static inline bool MEMB(_insert_uninit_)(Self *self, size_t idx, size_t n) {
    if (self->size + n > self->capacity) {
        size_t standard_cap = (self->capacity == 0) ? 8 : self->capacity * 2;
        size_t min_req_cap = self->size + n;
        size_t cap = (standard_cap > min_req_cap) ? standard_cap : min_req_cap;

        if (!MEMB(_reserve)(self, cap)) return false;
    }

    SelfValue *pos = self->data + idx;
    memmove(pos + n, pos, (self->size - idx) * sizeof(*pos));

    self->size += n;

    return true;
}

static inline bool MEMB(_insert)(Self *self, size_t idx, const SelfValue *arr, size_t n) {
    if (!MEMB(_insert_uninit_)(self, idx, n)) return false;

    memcpy(self->data + idx, arr, n * sizeof(SelfValue));
    return true;
}

static inline bool MEMB(_emplace_n)(Self *self, size_t idx, const SelfRaw *arr, size_t n) {
    if (!MEMB(_insert_uninit_)(self, idx, n)) return false;

    for (SelfValue *p = self->data + idx; n--; p++, arr++) {
        *p = value_from(*arr);
    }
    return true;
}

static inline const SelfValue *MEMB(_at)(const Self *self, size_t idx) {
    return self->data + idx;
}

static inline SelfValue *MEMB(_at_mut)(Self *self, size_t idx) {
    return self->data + idx;
}

#include "end.h"
