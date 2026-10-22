#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <template/types.h>

static size_t s_str_created_count = 0;
static size_t s_str_freed_count = 0;

static char *str(const char *s) {
    size_t len = strlen(s) + 1;

    char *dup = malloc(len * sizeof(char));
    if (!dup) return NULL;

    memcpy(dup, s, len * sizeof(char));

    s_str_created_count++;

    return dup;
}

static void str_free(char **str) {
    free(*str);
    s_str_freed_count++;
}

#define value_free str_free
#define T vec_str, char *
#include <template/vec.h>

static void test_vec_str(void) {
    vec_str v = {0};

    // Empty vector
    assert(vec_str_size(&v) == 0);
    assert(vec_str_capacity(&v) == 0);
    assert(vec_str_is_empty(&v));

    // Push
    vec_str_push(&v, str("Hello"));
    vec_str_push(&v, str("World"));

    assert(vec_str_size(&v) == 2);
    assert(vec_str_capacity(&v) == 8);
    assert(!vec_str_is_empty(&v));

    assert(strcmp(*vec_str_at(&v, 0), "Hello") == 0);
    assert(strcmp(*vec_str_at(&v, 1), "World") == 0);

    // Mutable access
    *vec_str_at_mut(&v, 1)[0] = 'w';
    assert(strcmp(*vec_str_at(&v, 1), "world") == 0);

    // Insert
    vec_str_value_type arr[] = {
        str("A"), str("B"), str("C"),
        str("D"), str("E"), str("F"),
        str("G"), str("H"), str("I")
    };

    vec_str_insert(&v, 1, arr, sizeof(arr) / sizeof(*arr));

    assert(vec_str_size(&v) == 11);
    assert(vec_str_capacity(&v) == 16);

    assert(strcmp(*vec_str_at(&v, 0), "Hello") == 0);
    assert(strcmp(*vec_str_at(&v, 1), "A") == 0);
    assert(strcmp(*vec_str_at(&v, 9), "I") == 0);
    assert(strcmp(*vec_str_at(&v, 10), "world") == 0);

    // Erase
    vec_str_erase(&v, 4, 5);

    assert(vec_str_size(&v) == 6);
    assert(vec_str_capacity(&v) == 16);

    assert(strcmp(*vec_str_at(&v, 2), "B") == 0);
    assert(strcmp(*vec_str_at(&v, 3), "C") == 0);
    assert(strcmp(*vec_str_at(&v, 4), "I") == 0);
    assert(strcmp(*vec_str_at(&v, 5), "world") == 0);

    // Shrink
    vec_str_shrink_to_fit(&v);
    assert(vec_str_size(&v) == 6);
    assert(vec_str_capacity(&v) == 6);

    // Clear
    vec_str_clear(&v);
    assert(vec_str_size(&v) == 0);
    assert(vec_str_capacity(&v) == 6);
    assert(vec_str_is_empty(&v));

    // Shrink an empty vector
    vec_str_shrink_to_fit(&v);
    assert(vec_str_capacity(&v) == 0);

    // Vector from an array
    v = MAKE(vec_str, { str("1"), str("2"), str("3") });

    assert(vec_str_size(&v) == 3);
    assert(vec_str_capacity(&v) == 3);

    // Drop
    vec_str_drop(&v);

    assert(vec_str_size(&v) == 0);
    assert(vec_str_capacity(&v) == 0);
    assert(vec_str_is_empty(&v));

    // All memory has been freed
    assert(s_str_created_count == s_str_freed_count);
}

int main(void) {
    test_vec_str();

    return 0;
}
