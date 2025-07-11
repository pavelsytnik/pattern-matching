/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2025 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "../src/vector.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static void test_int32_vector(void) {
    vector_of(int32_t) vec = NULL;
    assert(vector_size(vec) == 0);
    assert(vector_capacity(vec) == 0);
    assert(vector_elem_dtor(vec) == NULL);

    vector_push_back(vec, 1);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vector_size(vec) == 1);
    assert(vector_capacity(vec) == 1);

    vector_push_back(vec, 2);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vector_size(vec) == 2);
    assert(vector_capacity(vec) == 2);

    vector_push_back(vec, 3);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vec[2] == 3 && *vector_at(vec, 2) == 3);
    assert(vector_size(vec) == 3);
    assert(vector_capacity(vec) == 4);

    vector_push_back(vec, 4);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vec[2] == 3 && *vector_at(vec, 2) == 3);
    assert(vec[3] == 4 && *vector_at(vec, 3) == 4);
    assert(vector_size(vec) == 4);
    assert(vector_capacity(vec) == 4);

    vector_push_back(vec, 5);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vec[2] == 3 && *vector_at(vec, 2) == 3);
    assert(vec[3] == 4 && *vector_at(vec, 3) == 4);
    assert(vec[4] == 5 && *vector_at(vec, 4) == 5);
    assert(vector_size(vec) == 5);
    assert(vector_capacity(vec) == 8);

    vector_pop_back(vec);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vec[2] == 3 && *vector_at(vec, 2) == 3);
    assert(vec[3] == 4 && *vector_at(vec, 3) == 4);
    assert(vector_size(vec) == 4);
    assert(vector_capacity(vec) == 8);

    vector_pop_back(vec);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vec[2] == 3 && *vector_at(vec, 2) == 3);
    assert(vector_size(vec) == 3);
    assert(vector_capacity(vec) == 8);

    vector_shrink(vec);
    assert(vec[0] == 1 && *vector_at(vec, 0) == 1);
    assert(vec[1] == 2 && *vector_at(vec, 1) == 2);
    assert(vec[2] == 3 && *vector_at(vec, 2) == 3);
    assert(vector_size(vec) == 3);
    assert(vector_capacity(vec) == 3);

    vector_clear(vec);
    assert(vector_size(vec) == 0);
    assert(vector_capacity(vec) == 3);

    vector_free(vec);
    assert(vec == NULL);
}

int main(void) {
    test_int32_vector();
    return 0;
}
