#include <assert.h>
#include <string.h>

#include <pm.h>

static void test_bool_type(void) {
    assert(ty_bool->kind == TK_BOOL);
}

static void test_int_type(void) {
    assert(ty_int->kind == TK_INT);
}

static Type *make_option_type(const Type *ty) {
    VariantConstructor ctors[] = {
        {"Some", (const Type *[]){ty}, 1},
        {"None", NULL,                 0}
    };
    return new_variant(ctors, sizeof(ctors) / sizeof(*ctors));
}

static void test_option_int_type(void) {
    Type *opt_int_type = make_option_type(ty_int);

    assert(opt_int_type->kind == TK_VARIANT);
    assert(opt_int_type->variant.ctors_n == 2);
    assert(opt_int_type->variant.ctors != NULL);

    assert(strcmp(opt_int_type->variant.ctors[0].name, "Some") == 0);
    assert(opt_int_type->variant.ctors[0].args_n == 1);
    assert(opt_int_type->variant.ctors[0].args != NULL);
    assert(opt_int_type->variant.ctors[0].args[0] == ty_int);

    assert(strcmp(opt_int_type->variant.ctors[1].name, "None") == 0);
    assert(opt_int_type->variant.ctors[1].args_n == 0);
    assert(opt_int_type->variant.ctors[1].args == NULL);

    free_type(opt_int_type);
}

static void test_option_int_and_unit_type(void) {
    Type *opt_int_type = make_option_type(ty_int);
    // option_int_type is tested in the function above.

    Type *unit_type = new_tuple(NULL, 0);

    assert(unit_type->kind == TK_TUPLE);
    assert(unit_type->tuple.args_n == 0);
    assert(unit_type->tuple.args == NULL);

    Type *opt_int_and_unit_type = new_tuple((const Type *const []){opt_int_type, unit_type}, 2);

    assert(opt_int_and_unit_type->kind == TK_TUPLE);
    assert(opt_int_and_unit_type->tuple.args_n == 2);
    assert(opt_int_and_unit_type->tuple.args != NULL);

    assert(opt_int_and_unit_type->tuple.args[0] == opt_int_type);
    assert(opt_int_and_unit_type->tuple.args[1] == unit_type);

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
