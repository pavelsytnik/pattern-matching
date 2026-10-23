#include <assert.h>
#include <string.h>

#include <pm.h>
#include <template/types.h>

static const Type *type_by_id(const Compiler *comp, TypeId id) {
    return vec_type_at(&comp->types, id);
}

static void test_bool_type(void) {
    Type bool_type = ty_bool();

    assert(bool_type.kind == TK_BOOL);
}

static void test_int_type(void) {
    Type int_type = ty_int();

    assert(int_type.kind == TK_INT);
}

static Type make_option_type(TypeId ty) {
    return ty_variant(MAKE(vec_variant, {
        {"Some", MAKE(vec_typeid, {ty})},
        {"None", vec_typeid_init()}
    }));
}

static void test_option_int_type(void) {
    Compiler comp = compiler_new();
    TypeId int_type = type_new(&comp, ty_int());
    TypeId opt_int_type = type_new(&comp, make_option_type(int_type));

    const Type *ty = type_by_id(&comp, opt_int_type);
    const vec_variant *ctors = &ty->variant;

    assert(ty->kind == TK_VARIANT);
    assert(vec_variant_size(ctors) == 2);

    assert(vec_typeid_size(&vec_variant_at(ctors, 0)->args) == 1);
    assert(strcmp(vec_variant_at(ctors, 0)->name, "Some") == 0);
    assert(*vec_typeid_at(&vec_variant_at(ctors, 0)->args, 0) == int_type);

    assert(strcmp(vec_variant_at(ctors, 1)->name, "None") == 0);
    assert(vec_typeid_is_empty(&vec_variant_at(ctors, 1)->args));

    compiler_drop(&comp);
}

static void test_option_int_and_unit_type(void) {
    Compiler comp = compiler_new();
    TypeId int_type = type_new(&comp, ty_int());
    TypeId opt_int_type = type_new(&comp, make_option_type(int_type));
    // option_int_type is tested in the function above.

    TypeId unit_type = type_new(&comp, ty_tuple(vec_typeid_init()));

    {
        const Type *ty = type_by_id(&comp, unit_type);
        assert(ty->kind == TK_TUPLE);
        assert(vec_typeid_is_empty(&ty->tuple));
    }

    TypeId opt_int_and_unit_type = type_new(&comp, ty_tuple(
        MAKE(vec_typeid, { opt_int_type, unit_type })
    ));

    const Type *ty = type_by_id(&comp, opt_int_and_unit_type);
    const vec_typeid *args = &ty->tuple;

    assert(ty->kind == TK_TUPLE);
    assert(vec_typeid_size(args) == 2);
    assert(*vec_typeid_at(args, 0) == opt_int_type);
    assert(*vec_typeid_at(args, 1) == unit_type);

    compiler_drop(&comp);
}

int main(void) {
    test_bool_type();
    test_int_type();
    test_option_int_type();
    test_option_int_and_unit_type();

    return 0;
}
