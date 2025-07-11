/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2025 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef vector_h_
#define vector_h_

#include <stddef.h>
#include <stdlib.h>

typedef void (*vector_elem_dtor_ptr)(void *elem_ptr);

typedef struct {
    size_t capacity;
    size_t size;
    vector_elem_dtor_ptr elem_dtor;
} vector_metadata_;

#define vector_of(type) type *

#define vector_vec_to_meta_(vec) \
    (&((vector_metadata_ *)(void *)(vec))[-1])

#define vector_meta_to_vec_(meta) \
    ((void *)&((vector_metadata_ *)(meta))[1])

#define vector_capacity(vec) \
    ((vec) ? vector_vec_to_meta_((vec))->capacity : (size_t)0)

#define vector_size(vec) \
    ((vec) ? vector_vec_to_meta_((vec))->size : (size_t)0)

#define vector_elem_dtor(vec) \
    ((vec) ? vector_vec_to_meta_((vec))->elem_dtor : NULL)

#define vector_set_capacity_(vec, cap) do {           \
    if ((vec)) {                                      \
        vector_vec_to_meta_((vec))->capacity = (cap); \
    }                                                 \
} while (0)

#define vector_set_size_(vec, sz) do {           \
    if ((vec)) {                                 \
        vector_vec_to_meta_((vec))->size = (sz); \
    }                                            \
} while (0)

#define vector_set_elem_dtor_(vec, elem_dtor_fn) do {           \
    if ((vec)) {                                                \
        vector_vec_to_meta_((vec))->elem_dtor = (elem_dtor_fn); \
    }                                                           \
} while (0)

#define vector_begin(vec) \
    ((vec))

#define vector_end(vec) \
    ((vec) ? &(vec)[vector_size((vec))] : NULL)

#define vector_at(vec, i) (                                          \
    (vec) && (ptrdiff_t)(i) >= 0 && (size_t)(i) < vector_size((vec)) \
        ? &(vec)[(i)]                                                \
        : NULL                                                       \
)

#define vector_grow_(vec, n) do {                                            \
    size_t new_capacity__ = (n) * sizeof(*(vec)) + sizeof(vector_metadata_); \
                                                                             \
    if ((vec)) {                                                             \
        void *meta__ = vector_vec_to_meta_((vec));                           \
        void *new_memory__ = realloc(meta__, new_capacity__);                \
        (vec) = vector_meta_to_vec_(new_memory__);                           \
    } else {                                                                 \
        void *memory__ = malloc(new_capacity__);                             \
        (vec) = vector_meta_to_vec_(memory__);                               \
        vector_set_size_((vec), 0);                                          \
        vector_set_elem_dtor_((vec), NULL);                                  \
    }                                                                        \
                                                                             \
    vector_set_capacity_((vec), (n));                                        \
} while (0)

#define vector_reserve(vec, n) do {             \
    size_t capacity__ = vector_capacity((vec)); \
    if (capacity__ < (n)) {                     \
        vector_grow_((vec), (n));               \
    }                                           \
} while (0)

#define vector_init(vec, capacity, elem_dtor) do { \
    if (!(vec)) {                                  \
        vector_reserve((vec), (capacity));         \
        vector_set_elem_dtor_((vec), (elem_dtor)); \
    }                                              \
} while (0)

#define vector_compute_next_grow_(size) \
    ((size) ? (size) << 1 : 1)

#define vector_push_back(vec, value) do {                           \
    size_t capacity__ = vector_capacity((vec));                     \
    size_t size__ = vector_size((vec));                             \
                                                                    \
    if (capacity__ <= size__) {                                     \
        vector_grow_((vec), vector_compute_next_grow_(capacity__)); \
    }                                                               \
                                                                    \
    (vec)[size__] = (value);                                        \
    vector_set_size_((vec), size__ + 1);                            \
} while (0)

#define vector_pop_back(vec) do {                               \
    vector_elem_dtor_ptr elem_dtor__ = vector_elem_dtor((vec)); \
    size_t size__ = vector_size((vec));                         \
                                                                \
    if (elem_dtor__) {                                          \
        elem_dtor__(&(vec)[size__ - 1]);                        \
    }                                                           \
                                                                \
    vector_set_size_((vec), size__ - 1);                        \
} while (0)

#define vector_shrink(vec) do {                    \
    if ((vec)) {                                   \
        size_t shrunk_size__ = vector_size((vec)); \
        vector_grow_((vec), shrunk_size__);        \
    }                                              \
} while (0)

#endif // vector_h_
