/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "pm.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const Type *const ty_bool = &(const Type){TK_BOOL};
const Type *const ty_int = &(const Type){TK_INT};

Type *new_tuple(vec_typeid args) {
    Type *ty = malloc(sizeof(Type));
    if (!ty) return NULL;

    ty->kind = TK_TUPLE;
    ty->tuple = args;

    return ty;
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

Type *new_variant(vec_variant ctors) {
    Type *ty = malloc(sizeof(Type));
    if (!ty) return NULL;

    ty->kind = TK_VARIANT;
    ty->variant = ctors;

    return ty;
}

void free_type(Type *ty) {
    if (ty->kind == TK_TUPLE) {
        vec_typeid_drop(&ty->tuple);
    } else if (ty->kind == TK_VARIANT) {
        vec_variant_drop(&ty->variant);
    }
}

static size_t variant_ctor_idx(const Type *ty, const char *name) {
    size_t n = vec_variant_size(&ty->variant);

    for (size_t i = 0; i < n; i++) {
        if (strcmp(vec_variant_at(&ty->variant, i)->name, name) == 0) {
            return i;
        }
    }
    return (size_t)-1;
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

Pattern pt_tuple(const Type *ty, vec_pattern args) {
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

Pattern pt_variant(const Type *ty, const char *name, vec_pattern args) {
    size_t ctor_idx = variant_ctor_idx(ty, name);

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
