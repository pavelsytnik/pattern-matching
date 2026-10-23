#include <assert.h>
#include <string.h>

#include <pm.h>
#include <template/types.h>

static void test_bool_type(void) {
    assert(ty_bool->kind == TK_BOOL);
}

static void test_int_type(void) {
    assert(ty_int->kind == TK_INT);
}

static Type *make_option_type(const Type *ty) {
    return new_variant(MAKE(vec_variant, {
        {"Some", MAKE(vec_typeid, {ty})},
        {"None", vec_typeid_init()}
    }));
}

static void test_option_int_type(void) {
    Type *opt_int_type = make_option_type(ty_int);

    const vec_variant *ctors = &opt_int_type->variant;

    assert(opt_int_type->kind == TK_VARIANT);
    assert(vec_variant_size(ctors) == 2);

    assert(vec_typeid_size(&vec_variant_at(ctors, 0)->args) == 1);
    assert(strcmp(vec_variant_at(ctors, 0)->name, "Some") == 0);
    assert(*vec_typeid_at(&vec_variant_at(ctors, 0)->args, 0) == ty_int);

    assert(strcmp(vec_variant_at(ctors, 1)->name, "None") == 0);
    assert(vec_typeid_is_empty(&vec_variant_at(ctors, 1)->args));

    free_type(opt_int_type);
}

static void test_option_int_and_unit_type(void) {
    Type *opt_int_type = make_option_type(ty_int);
    // option_int_type is tested in the function above.

    Type *unit_type = new_tuple(vec_typeid_init());

    assert(unit_type->kind == TK_TUPLE);
    assert(vec_typeid_is_empty(&unit_type->tuple));

    Type *opt_int_and_unit_type = new_tuple(MAKE(vec_typeid, {
        opt_int_type, unit_type
    }));

    const vec_typeid *args = &opt_int_and_unit_type->tuple;

    assert(opt_int_and_unit_type->kind == TK_TUPLE);
    assert(vec_typeid_size(args) == 2);
    assert(*vec_typeid_at(args, 0) == opt_int_type);
    assert(*vec_typeid_at(args, 1) == unit_type);

    free_type(opt_int_and_unit_type);
    free_type(unit_type);
    free_type(opt_int_type);
}

int main(void) {
    test_bool_type();
    test_int_type();
    test_option_int_type();
    test_option_int_and_unit_type();

    return 0;
}
