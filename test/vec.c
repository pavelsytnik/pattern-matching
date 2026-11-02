#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <util/common.h>

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

#define t_value_drop str_free
#define t_value_from str
#define t_value_clone str
#define T vec_str, char *
#include <util/vec.h>

static void test_vec_str(void) {
    vec_str v = vec_str_new();

    // Empty vector
    assert(vec_str_size(&v) == 0);
    assert(vec_str_capacity(&v) == 0);
    assert(vec_str_is_empty(&v));

    // Push & Emplace
    vec_str_push(&v, str("Hello"));
    vec_str_emplace(&v, "World");

    assert(vec_str_size(&v) == 2);
    assert(vec_str_capacity(&v) == 8);
    assert(!vec_str_is_empty(&v));

    assert(strcmp(*vec_str_at(&v, 0), "Hello") == 0);
    assert(strcmp(*vec_str_at(&v, 1), "World") == 0);

    // Mutable access
    *vec_str_at_mut(&v, 1)[0] = 'w';
    assert(strcmp(*vec_str_at(&v, 1), "world") == 0);

    // Insert
    vec_str_value arr[] = {
        str("A"), str("B"), str("C"),
        str("D"), str("E"), str("F"),
        str("G"), str("H"), str("I")
    };

    vec_str_insert_n(&v, 1, arr, sizeof(arr) / sizeof(*arr));

    assert(vec_str_size(&v) == 11);
    assert(vec_str_capacity(&v) == 16);

    assert(strcmp(*vec_str_at(&v, 0), "Hello") == 0);
    assert(strcmp(*vec_str_at(&v, 1), "A") == 0);
    assert(strcmp(*vec_str_at(&v, 9), "I") == 0);
    assert(strcmp(*vec_str_at(&v, 10), "world") == 0);

    // Erase
    vec_str_erase_n(&v, 4, 5);

    assert(vec_str_size(&v) == 6);
    assert(vec_str_capacity(&v) == 16);

    assert(strcmp(*vec_str_at(&v, 2), "B") == 0);
    assert(strcmp(*vec_str_at(&v, 3), "C") == 0);
    assert(strcmp(*vec_str_at(&v, 4), "I") == 0);
    assert(strcmp(*vec_str_at(&v, 5), "world") == 0);

    // Emplace N
    vec_str_raw raw[] = { "D", "E", "F", "G", "H" };

    vec_str_emplace_n(&v, 4, raw, sizeof(raw) / sizeof(*raw));

    assert(vec_str_size(&v) == 11);
    assert(vec_str_capacity(&v) == 16);

    assert(strcmp(*vec_str_at(&v, 4), "D") == 0);
    assert(strcmp(*vec_str_at(&v, 8), "H") == 0);

    // Shrink
    vec_str_shrink_to_fit(&v);
    assert(vec_str_size(&v) == 11);
    assert(vec_str_capacity(&v) == 11);

    // Clear
    vec_str_clear(&v);
    assert(vec_str_size(&v) == 0);
    assert(vec_str_capacity(&v) == 11);
    assert(vec_str_is_empty(&v));

    // Shrink an empty vector
    vec_str_shrink_to_fit(&v);
    assert(vec_str_capacity(&v) == 0);

    // Vector from an array
    vec_str_take(&v, x_make(vec_str, { "1", "2", "3" }));

    assert(vec_str_size(&v) == 3);
    assert(vec_str_capacity(&v) == 3);

    // Clone
    {
        vec_str v2 = vec_str_clone(&v);

        assert(vec_str_size(&v2) == 3);
        assert(vec_str_capacity(&v2) == 3);
        assert(strcmp(*vec_str_at(&v2, 0), "1") == 0);
        assert(strcmp(*vec_str_at(&v2, 2), "3") == 0);

        vec_str_drop(&v2);
    }

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
