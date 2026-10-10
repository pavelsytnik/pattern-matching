#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pm.h>

static void assert_ctor_pattern(const Pattern *pt, ConstructorKind ctor_kind, size_t args_n) {
    assert(pt->kind == PK_CTOR);
    assert(pt->ctor.ctor.kind == ctor_kind);
    assert(pt->ctor.args_n == args_n);

    if (pt->ctor.args_n == 0) {
        assert(pt->ctor.args == NULL);
    } else {
        assert(pt->ctor.args != NULL);
    }
}

static void assert_false_pattern(const Pattern *pt) {
    assert_ctor_pattern(pt, CK_FALSE, 0);
}

static void assert_true_pattern(const Pattern *pt) {
    assert_ctor_pattern(pt, CK_TRUE, 0);
}

static void assert_int_pattern(const Pattern *pt, int32_t i) {
    assert_ctor_pattern(pt, CK_INT, 0);
    assert(pt->ctor.ctor.integer == i);
}

static void assert_wildcard_pattern(const Pattern *pt) {
    assert(pt->kind == PK_WILDCARD);
}

static void test_false_pattern(void) {
    Pattern pt = pt_false();
    assert_false_pattern(&pt);
}

static void test_true_pattern(void) {
    Pattern pt = pt_true();
    assert_true_pattern(&pt);
}

static void test_int_pattern(void) {
    Pattern pt = pt_int(13);
    assert_int_pattern(&pt, 13);
}

static void test_int_bool_tuple_pattern(void) {
    Type *int_bool_tuple = new_tuple((const Type * []){ty_int, ty_bool}, 2);

    Pattern pt = pt_tuple(int_bool_tuple, (Pattern []){
        pt_int(123),
        pt_true()
    });

    assert_ctor_pattern(&pt, CK_TUPLE, 2);
    assert(pt.ctor.ctor.tuple.args == int_bool_tuple->tuple.args);
    assert(pt.ctor.ctor.tuple.args_n == int_bool_tuple->tuple.args_n);

    assert_int_pattern(&pt.ctor.args[0], 123);
    assert_true_pattern(&pt.ctor.args[1]);

    free_type(int_bool_tuple);
}

static void test_result_variant_pattern(void) {
    Type *int_int_tuple = new_tuple((const Type * []){ty_int, ty_int}, 2);

    Type *result_type = new_variant((VariantConstructor []){
        {"Ok",  (const Type * []){int_int_tuple}, 1},
        {"Err", (const Type * []){ty_int},        1}
    }, 2);

    Pattern ok_pt = pt_variant(result_type, "Ok", (Pattern []){
        pt_tuple(int_int_tuple, (Pattern []){
            pt_wildcard(),
            pt_int(88)
        })
    });

    assert_ctor_pattern(&ok_pt, CK_VARIANT_CTOR, 1);
    assert(ok_pt.ctor.ctor.variant_ctor == &result_type->variant.ctors[0]);

    const Pattern *tuple_pt = &ok_pt.ctor.args[0];

    assert_ctor_pattern(tuple_pt, CK_TUPLE, 2);
    assert_wildcard_pattern(&tuple_pt->ctor.args[0]);
    assert_int_pattern(&tuple_pt->ctor.args[1], 88);

    // TODO: Test the `Err` constructor.

    free_type(result_type);
    free_type(int_int_tuple);
}

static void test_wildcard_pattern(void) {
    Pattern pt = pt_wildcard();
    assert_wildcard_pattern(&pt);
}

int main(void) {
    test_false_pattern();
    test_true_pattern();
    test_int_pattern();
    test_int_bool_tuple_pattern();
    test_result_variant_pattern();
    test_wildcard_pattern();

    return 0;
}
