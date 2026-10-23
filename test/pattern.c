#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pm.h>
#include <template/types.h>

static void assert_ctor_pattern(const Pattern *pt, ConstructorKind ctor_kind, size_t args_n) {
    assert(pt->kind == PK_CTOR);
    assert(pt->ctor.of.kind == ctor_kind);
    assert(vec_pattern_size(&pt->ctor.args) == args_n);
}

static void assert_false_pattern(const Pattern *pt) {
    assert_ctor_pattern(pt, CK_FALSE, 0);
}

static void assert_true_pattern(const Pattern *pt) {
    assert_ctor_pattern(pt, CK_TRUE, 0);
}

static void assert_int_pattern(const Pattern *pt, int32_t i) {
    assert_ctor_pattern(pt, CK_INT, 0);
    assert(pt->ctor.of.integer == i);
}

static void assert_wildcard_pattern(const Pattern *pt) {
    assert(pt->kind == PK_WILDCARD);
}

static void test_false_pattern(void) {
    Pattern pt = pt_false();
    assert_false_pattern(&pt);
    pt_drop(&pt);
}

static void test_true_pattern(void) {
    Pattern pt = pt_true();
    assert_true_pattern(&pt);
    pt_drop(&pt);
}

static void test_int_pattern(void) {
    Pattern pt = pt_int(13);
    assert_int_pattern(&pt, 13);
    pt_drop(&pt);
}

static void test_int_bool_tuple_pattern(void) {
    Compiler comp = compiler_new();
    TypeId bool_type = type_new(&comp, ty_bool());
    TypeId int_type = type_new(&comp, ty_int());
    TypeId int_bool_tuple = type_new(&comp, ty_tuple(
        MAKE(vec_typeid, {int_type, bool_type})
    ));

    Pattern pt = pt_tuple(int_bool_tuple, MAKE(vec_pattern, {
        pt_int(123),
        pt_true()
    }));

    assert_ctor_pattern(&pt, CK_TUPLE, 2);
    assert(pt.ctor.of.tuple == int_bool_tuple);

    assert_int_pattern(vec_pattern_at(&pt.ctor.args, 0), 123);
    assert_true_pattern(vec_pattern_at(&pt.ctor.args, 1));

    pt_drop(&pt);
    compiler_drop(&comp);
}

static void test_result_variant_pattern(void) {
    Compiler comp = compiler_new();
    TypeId int_type = type_new(&comp, ty_int());
    TypeId int_int_tuple = type_new(&comp, ty_tuple(
        MAKE(vec_typeid, {int_type, int_type})
    ));

    TypeId result_type = type_new(&comp, ty_variant(
        MAKE(vec_variant, {
            {"Ok",  MAKE(vec_typeid, {int_int_tuple})},
            {"Err", MAKE(vec_typeid, {int_type})}
        })
    ));

    Pattern ok_pt = pt_variant(result_type, 0, MAKE(vec_pattern, {
        pt_tuple(int_int_tuple, MAKE(vec_pattern, {
            pt_wildcard(),
            pt_int(88)
        }))
    }));

    assert_ctor_pattern(&ok_pt, CK_VARIANT, 1);
    assert(ok_pt.ctor.of.variant.type == result_type);
    assert(ok_pt.ctor.of.variant.ctor_idx == 0);

    const Pattern *tuple_pt = vec_pattern_at(&ok_pt.ctor.args, 0);

    assert_ctor_pattern(tuple_pt, CK_TUPLE, 2);
    assert_wildcard_pattern(vec_pattern_at(&tuple_pt->ctor.args, 0));
    assert_int_pattern(vec_pattern_at(&tuple_pt->ctor.args, 1), 88);

    // TODO: Test the `Err` constructor.

    pt_drop(&ok_pt);
    compiler_drop(&comp);
}

static void test_wildcard_pattern(void) {
    Pattern pt = pt_wildcard();
    assert_wildcard_pattern(&pt);
    pt_drop(&pt);
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
