#include <assert.h>

#include <pm.h>
#include <template/common.h>

static void test_int(void) {
    Compiler c = compiler_new();
    TypeId i = new_type(&c, ty_int());

    Decision d = compile(&c, MAKE(vec_row, {
        row_new(MAKE(vec_column, {
            column_new(i, pt_int(666))
        }), 11),
        row_new(MAKE(vec_column, {
            column_new(i, pt_int(999))
        }), 22),
        row_new(MAKE(vec_column, {
            column_new(i, pt_wildcard())
        }), 33)
    }));

    assert(d.kind == DK_BRANCH);
    assert(vec_case_size(&d.branch.cases) == 2);
    assert(d.branch.fallback != NULL);

    assert(vec_case_at(&d.branch.cases, 0)->decision.kind == DK_LEAF);
    assert(vec_case_at(&d.branch.cases, 0)->ctor.integer == 666);

    assert(vec_case_at(&d.branch.cases, 1)->decision.kind == DK_LEAF);
    assert(vec_case_at(&d.branch.cases, 1)->ctor.integer == 999);

    assert(d.branch.fallback->kind == DK_LEAF);
    assert(d.branch.fallback->leaf == 33);

    decision_drop(&d);
    compiler_drop(&c);
}

static void test_option_int(void) {
    Compiler c = compiler_new();

    TypeId i = new_type(&c, ty_int());
    TypeId v = new_type(&c, ty_variant(MAKE(vec_variant, {
        { "Some", MAKE(vec_typeid, { i }) },
        { "None", vec_typeid_init() }
    })));

    Decision d = compile(&c, MAKE(vec_row, {
        row_new(MAKE(vec_column, {
            column_new(v, pt_variant(v, 1, vec_pattern_init()))
        }), 987),
        row_new(MAKE(vec_column, {
            column_new(v, pt_variant(v, 0, MAKE(vec_pattern, {
                pt_int(888)
            })))
        }), 123)
    }));

    assert(d.kind = DK_BRANCH);
    assert(vec_case_size(&d.branch.cases) == 2);
    assert(d.branch.fallback == NULL);

    {
        const vec_case *v_cases = &d.branch.cases;
        const Decision *v_fallback = d.branch.fallback;
        const Case *v_some = vec_case_at(v_cases, 1);
        const Case *v_none = vec_case_at(v_cases, 0);

        {
            Constructor some = cs_variant(v, 0);
            Constructor none = cs_variant(v, 1);

            assert(cs_eq(&v_none->ctor, &none));
            assert(cs_eq(&v_some->ctor, &some));
        }

        assert(v_none->decision.kind == DK_LEAF);
        assert(v_none->decision.leaf == 987);

        assert(v_some->decision.kind == DK_BRANCH);
        assert(vec_case_size(&v_some->decision.branch.cases) == 1);
        assert(v_some->decision.branch.fallback != NULL);

        {
            const vec_case *v_some_cases = &v_some->decision.branch.cases;
            const Decision *v_some_fallback = v_some->decision.branch.fallback;
            const Case *v_some_888 = vec_case_at(v_some_cases, 0);

            {
                Constructor _888 = cs_int(888);
                assert(cs_eq(&v_some_888->ctor, &_888));
            }

            assert(v_some_888->decision.kind == DK_LEAF);
            assert(v_some_888->decision.leaf == 123);

            assert(v_some_fallback->kind == DK_FAILURE);
        }
    }

    decision_drop(&d);
    compiler_drop(&c);
}

int main(void) {
    test_int();
    test_option_int();

    return 0;
}
