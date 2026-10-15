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

Type *new_tuple(const Type *const *args, size_t n) {
    Type *ty = malloc(sizeof(Type));
    if (!ty) return NULL;

    const Type **args_copy = NULL;

    if (n > 0) {
        args_copy = malloc(n * sizeof(*args));
        if (!args_copy) {
            free(ty);
            return NULL;
        }

        memcpy(args_copy, args, n * sizeof(*args));
    }

    ty->kind = TK_TUPLE;
    ty->tuple.args = args_copy;
    ty->tuple.args_n = n;

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

static int copy_variant_constructor(const VariantConstructor *src, VariantConstructor *dst) {
    dst->name = copy_string(src->name);
    if (!dst->name) return 1;

    dst->args = NULL;
    dst->args_n = src->args_n;

    if (dst->args_n > 0) {
        dst->args = malloc(dst->args_n * sizeof(*dst->args));
        if (!dst->args) {
            free(dst->name);
            return 1;
        }

        memcpy(dst->args, src->args, dst->args_n * sizeof(*dst->args));
    }

    return 0;
}

Type *new_variant(const VariantConstructor *ctors, size_t n) {
    Type *ty = malloc(sizeof(Type));
    if (!ty) return NULL;

    VariantConstructor *ctors_copy = NULL;

    if (n > 0) {
        ctors_copy = malloc(n * sizeof(*ctors));
        if (!ctors_copy) {
            free(ty);
            return NULL;
        }

        for (size_t i = 0; i < n; i++) {
            if (copy_variant_constructor(&ctors[i], &ctors_copy[i]) != 0) {
                free(ctors_copy);
                free(ty);
                return NULL;
            }
        }
    }

    ty->kind = TK_VARIANT;
    ty->variant.ctors = ctors_copy;
    ty->variant.ctors_n = n;

    return ty;
}

void free_type(Type *ty) {
    if (ty->kind == TK_TUPLE) {
        free(ty->tuple.args);
    } else if (ty->kind == TK_VARIANT) {
        for (size_t i = 0; i < ty->variant.ctors_n; i++) {
            free(ty->variant.ctors[i].args);
            free(ty->variant.ctors[i].name);
        }
        free(ty->variant.ctors);
    }
}

static size_t variant_ctor_idx(const Type *ty, const char *name) {
    for (size_t i = 0; i < ty->variant.ctors_n; i++) {
        if (strcmp(ty->variant.ctors[i].name, name) == 0) {
            return i;
        }
    }
    return (size_t)-1;
}

Pattern pt_false(void) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .ctor = {
                .kind = CK_FALSE
            },
            .args = NULL,
            .args_n = 0
        }
    };
}

Pattern pt_true(void) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .ctor = {
                .kind = CK_TRUE
            },
            .args = NULL,
            .args_n = 0
        }
    };
}

Pattern pt_int(int32_t i) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .ctor = {
                .kind = CK_INT,
                .integer = i
            },
            .args = NULL,
            .args_n = 0
        }
    };
}

Pattern pt_tuple(const Type *ty, Pattern *args) {
    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .ctor = {
                .kind = CK_TUPLE,
                .tuple = ty
            },
            .args = args,
            .args_n = ty->tuple.args_n
        }
    };
}

Pattern pt_variant(const Type *ty, const char *name, Pattern *args) {
    size_t ctor_idx = variant_ctor_idx(ty, name);

    return (Pattern){
        .kind = PK_CTOR,
        .ctor = {
            .ctor = {
                .kind = CK_VARIANT_CTOR,
                .variant = {
                    .type = ty,
                    .ctor_idx = ctor_idx
                }
            },
            .args = args,
            .args_n = ty->variant.ctors[ctor_idx].args_n
        }
    };
}

Pattern pt_wildcard(void) {
    return (Pattern){
        .kind = PK_WILDCARD
    };
}
