/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "pm.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Type ty_bool(void) {
    return (Type){
        .kind = TK_BOOL
    };
}

Type ty_int(void) {
    return (Type){
        .kind = TK_INT
    };
}

Type ty_tuple(vec_typeid args) {
    return (Type){
        .kind = TK_TUPLE,
        .tuple = args
    };
}

Type ty_variant(vec_variant ctors) {
    return (Type){
        .kind = TK_VARIANT,
        .variant = ctors
    };
}

static char *copy_string(const char *s) {
    if (!s) return NULL;

    size_t n = strlen(s) + 1;

    char *cp = malloc(n);
    if (!cp) return NULL;

    memcpy(cp, s, n * sizeof(*s));
    return cp;
}

VariantConstructor variant_ctor(VariantConstructor ctor) {
    return (VariantConstructor){
        .name = copy_string(ctor.name),
        .args = vec_typeid_move(&ctor.args)
    };
}

void free_variant_ctor(VariantConstructor *ctor) {
    free(ctor->name);
    vec_typeid_drop(&ctor->args);
}

void free_type(Type *ty) {
    if (ty->kind == TK_TUPLE) {
        vec_typeid_drop(&ty->tuple);
    } else if (ty->kind == TK_VARIANT) {
        vec_variant_drop(&ty->variant);
    }
}

Constructor cs_false(void) {
    return (Constructor){
        .kind = CK_FALSE
    };
}

Constructor cs_true(void) {
    return (Constructor){
        .kind = CK_TRUE
    };
}

Constructor cs_int(int32_t i) {
    return (Constructor){
        .kind = CK_INT,
        .integer = i
    };
}

Constructor cs_tuple(TypeId ty) {
    return (Constructor){
        .kind = CK_TUPLE,
        .tuple = ty
    };
}

Constructor cs_variant(TypeId ty, size_t ctor_idx) {
    return (Constructor){
        .kind = CK_VARIANT,
        .variant = {
            .type = ty,
            .ctor_idx = ctor_idx
        }
    };
}

Pattern pt_false(void) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .of = {
                .kind = CK_FALSE
            },
            .args = vec_pattern_init()
        }
    };
}

Pattern pt_true(void) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .of = {
                .kind = CK_TRUE
            },
            .args = vec_pattern_init()
        }
    };
}

Pattern pt_int(int32_t i) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .of = {
                .kind = CK_INT,
                .integer = i
            },
            .args = vec_pattern_init()
        }
    };
}

Pattern pt_tuple(TypeId ty, vec_pattern args) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .of = {
                .kind = CK_TUPLE,
                .tuple = ty
            },
            .args = args
        }
    };
}

Pattern pt_variant(TypeId ty, size_t ctor_idx, vec_pattern args) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .of = {
                .kind = CK_VARIANT,
                .variant = {
                    .type = ty,
                    .ctor_idx = ctor_idx
                }
            },
            .args = args
        }
    };
}

Pattern pt_wildcard(void) {
    return (Pattern){
        .kind = PK_WILDCARD
    };
}

Pattern pt_clone(Pattern pt) {
    if (pt.kind == PK_CTOR && (pt.ctor.of.kind == CK_TUPLE || pt.ctor.of.kind == CK_VARIANT)) {
        pt.ctor.args = vec_pattern_clone(&pt.ctor.args);
    }
    return pt;
}

void pt_drop(Pattern *pt) {
    if (pt->kind != PK_CTOR) return;

    switch (pt->ctor.of.kind) {
    case CK_TUPLE:
    case CK_VARIANT:
        vec_pattern_drop(&pt->ctor.args);
        break;
    }
}

Decision dc_leaf(Body body) {
    return (Decision){
        .kind = DK_LEAF,
        .leaf = body
    };
}

Decision dc_failure(void) {
    return (Decision){
        .kind = DK_FAILURE
    };
}

Decision dc_branch(vec_case cases, Decision *fallback) {
    return (Decision){
        .kind = DK_BRANCH,
        .branch = {
            .cases = cases,
            .fallback = fallback
        }
    };
}

Column column_clone(Column col) {
    col.pattern = pt_clone(col.pattern);
    return col;
}

void column_drop(Column *col) {
    pt_drop(&col->pattern);
}

void row_drop(Row *row) {
    vec_column_drop(&row->columns);
}

void case_drop(Case *kase) {
    decision_drop(&kase->decision);
}

void decision_drop(Decision *decision) {
    if (decision->kind == DK_BRANCH) {
        vec_case_drop(&decision->branch.cases);
        free(decision->branch.fallback);
    }
}

Compiler compiler_new(void) {
    return (Compiler){
        .types = vec_type_init()
    };
}

void compiler_drop(Compiler *compiler) {
    vec_type_drop(&compiler->types);
}

TypeId type_new(Compiler *compiler, Type ty) {
    TypeId id = vec_type_size(&compiler->types);
    vec_type_push(&compiler->types, ty);
    return id;
}

const Type *type_get(const Compiler *compiler, TypeId id) {
    return vec_type_at(&compiler->types, id);
}

Column column_new(TypeId ty, Pattern pt) {
    return (Column){
        .type = ty,
        .pattern = pt
    };
}

Row row_new(vec_column cols, Body body) {
    return (Row){
        .columns = cols,
        .body = body
    };
}

Case case_new(Constructor ctor, Decision decision) {
    return (Case){
        .ctor = ctor,
        .decision = decision
    };
}

bool cs_eq(const Constructor *c1, const Constructor *c2) {
    if (c1 == c2) return true;
    if (c1->kind != c2->kind) return false;

    ConstructorKind kind = c1->kind;

    switch (kind) {
    case CK_FALSE:
    case CK_TRUE:
        return true;
    case CK_INT:
        return c1->integer == c2->integer;
    case CK_TUPLE:
        return c1->tuple == c2->tuple;
    case CK_VARIANT:
        return c1->variant.type == c2->variant.type
            && c1->variant.ctor_idx == c2->variant.ctor_idx;
    }

    return false;
}

static bool ctors_contain(const vec_ctor *ctors, Constructor ctor) {
    for (size_t i = 0; i < vec_ctor_size(ctors); i++) {
        if (cs_eq(vec_ctor_at(ctors, i), &ctor)) {
            return true;
        }
    }
    return false;
}

static const Column *rows_first_column_at(const vec_row *rows, size_t idx) {
    return vec_column_at(&vec_row_at(rows, idx)->columns, 0);
}

static vec_ctor collect_ctors(const Compiler *compiler, const vec_row *rows, bool *fallback) {
    TypeId type_id = rows_first_column_at(rows, 0)->type;
    const Type *type = type_get(compiler, type_id);

    vec_ctor ctors = vec_ctor_init();

    switch (type->kind) {
    case TK_BOOL:
        vec_ctor_take(&ctors, MAKE(vec_ctor, {
            cs_false(),
            cs_true()
        }));
        *fallback = false;
        break;
    case TK_INT:
    case TK_VARIANT:
        for (size_t i = 0; i < vec_row_size(rows); i++) {
            const Pattern *pt = &rows_first_column_at(rows, i)->pattern;
            if (pt->kind != PK_CTOR) continue;

            Constructor ctor = pt->ctor.of;
            if (!ctors_contain(&ctors, ctor)) vec_ctor_push(&ctors, ctor);
        }

        *fallback = type->kind == TK_INT ||
            vec_ctor_size(&ctors) < vec_variant_size(&type->variant);
        break;
    case TK_TUPLE:
        vec_ctor_take(&ctors, MAKE(vec_ctor, { cs_tuple(type_id) }));
        *fallback = false;
        break;
    }

    return ctors;
}

static const vec_typeid *ctor_args(const Compiler *compiler, Constructor ctor) {
    const vec_typeid *args = NULL;

    if (ctor.kind == CK_TUPLE) {
        const Type *type = type_get(compiler, ctor.tuple);
        args = &type->tuple;
    } else if (ctor.kind == CK_VARIANT) {
        const Type *type = type_get(compiler, ctor.variant.type);
        const VariantConstructor *variant = vec_variant_at(&type->variant, ctor.variant.ctor_idx);
        args = &variant->args;
    }

    return args;
}

static vec_row specialize_rows(const Compiler *compiler, const vec_row *rows, Constructor ctor) {
    vec_row s = vec_row_init();

    for (size_t i = 0; i < vec_row_size(rows); i++) {
        const Row *row = vec_row_at(rows, i);
        const Pattern *pt = &vec_column_at(&row->columns, 0)->pattern;

        if (pt->kind != PK_WILDCARD && (pt->kind != PK_CTOR || !cs_eq(&pt->ctor.of, &ctor))) {
            continue;
        }

        const vec_typeid *args = ctor_args(compiler, ctor);
        size_t arg_n = (args) ? vec_typeid_size(args) : 0;

        vec_column cols = vec_column_init();

        for (size_t j = 0; j < arg_n; j++) {
            TypeId t = *vec_typeid_at(args, j);
            Pattern q;

            if (pt->kind == PK_CTOR) {
                q = pt_clone(*vec_pattern_at(&pt->ctor.args, j));
            } else {
                q = pt_wildcard();
            }

            vec_column_push(&cols, column_new(t, q));
        }

        for (size_t j = 1; j < vec_column_size(&row->columns); j++) {
            const Column *col = vec_column_at(&row->columns, j);
            vec_column_push(&cols, column_new(col->type, pt_clone(col->pattern)));
        }

        vec_row_push(&s, row_new(vec_column_move(&cols), row->body));
    }

    return s;
}

static vec_row fallback_rows(const Compiler *compiler, const vec_row *rows) {
    vec_row d = vec_row_init();

    for (size_t i = 0; i < vec_row_size(rows); i++) {
        const Row *row = vec_row_at(rows, i);
        const Pattern *pt = &vec_column_at(&row->columns, 0)->pattern;

        if (pt->kind != PK_WILDCARD) {
            continue;
        }

        vec_column cols = vec_column_init();

        for (size_t j = 1; j < vec_column_size(&row->columns); j++) {
            const Column *col = vec_column_at(&row->columns, j);
            vec_column_push(&cols, column_new(col->type, pt_clone(col->pattern)));
        }

        vec_row_push(&d, row_new(vec_column_move(&cols), row->body));
    }

    return d;
}

Decision compile(const Compiler *compiler, vec_row rows) {
    if (vec_row_is_empty(&rows)) {
        vec_row_drop(&rows);

        return dc_failure();
    }

    if (vec_column_is_empty(&vec_row_at(&rows, 0)->columns)) {
        Body body = vec_row_at(&rows, 0)->body;
        vec_row_drop(&rows);

        return dc_leaf(body);
    }

    bool fallback_needed;
    vec_ctor ctors = collect_ctors(compiler, &rows, &fallback_needed);

    vec_case cases = vec_case_init();
    Decision *fallback = NULL;

    for (size_t i = 0; i < vec_ctor_size(&ctors); i++) {
        Constructor ctor = *vec_ctor_at(&ctors, i);
        vec_row s = specialize_rows(compiler, &rows, ctor);

        vec_case_push(&cases, case_new(ctor, compile(compiler, vec_row_move(&s))));
    }

    if (fallback_needed) {
        fallback = malloc(sizeof(*fallback));
        vec_row d = fallback_rows(compiler, &rows);

        *fallback = compile(compiler, vec_row_move(&d));
    }

    vec_ctor_drop(&ctors);
    vec_row_drop(&rows);

    return dc_branch(cases, fallback);
}
