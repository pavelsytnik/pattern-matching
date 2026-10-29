/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "pm.h"

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
