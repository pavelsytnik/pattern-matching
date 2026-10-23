/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef pm__h_
#define pm__h_

#include <stddef.h>
#include <stdint.h>

#include "template/types.h"

typedef enum TypeKind TypeKind;
typedef enum ConstructorKind ConstructorKind;
typedef enum PatternKind PatternKind;

typedef struct Type Type;
typedef struct VariantConstructor VariantConstructor;
typedef struct Constructor Constructor;
typedef struct Pattern Pattern;
typedef struct Compiler Compiler;

typedef size_t TypeId;

DECLARE_VEC(vec_type, Type);
DECLARE_VEC(vec_typeid, TypeId);
DECLARE_VEC(vec_variant, VariantConstructor);
DECLARE_VEC(vec_pattern, Pattern);

enum TypeKind {
    TK_BOOL,
    TK_INT,
    TK_TUPLE,
    TK_VARIANT
};

enum ConstructorKind {
    CK_FALSE,
    CK_TRUE,
    CK_INT,
    CK_TUPLE,
    CK_VARIANT
};

enum PatternKind {
    PK_CTOR,
    PK_WILDCARD
};

struct VariantConstructor {
    char *name;
    vec_typeid args;
};

struct Type {
    TypeKind kind;

    union {
        vec_typeid tuple;
        vec_variant variant;
    };
};

struct Constructor {
    ConstructorKind kind;

    union {
        int32_t integer;
        TypeId tuple;
        struct {
            TypeId type;
            size_t ctor_idx;
        } variant;
    };
};

struct Pattern {
    PatternKind kind;

    union {
        struct {
            Constructor of;
            vec_pattern args;
        } ctor;
    };
};

struct Compiler {
    vec_type types;
};

void free_type(Type *ty);

#define DECLARED
#define value_free free_type
#define T vec_type, Type
#include "template/vec.h"

#define DECLARED
#define T vec_typeid, TypeId
#include "template/vec.h"

VariantConstructor variant_ctor(VariantConstructor ctor);
void free_variant_ctor(VariantConstructor *ctor);

#define DECLARED
#define value_from variant_ctor
#define value_free free_variant_ctor
#define T vec_variant, VariantConstructor
#include "template/vec.h"

void pt_drop(Pattern *pt);

#define DECLARED
#define value_free pt_drop
#define T vec_pattern, Pattern
#include "template/vec.h"

Type ty_bool(void);
Type ty_int(void);
Type ty_tuple(vec_typeid args);
Type ty_variant(vec_variant ctors);

Pattern pt_false(void);
Pattern pt_true(void);
Pattern pt_int(int32_t i);
Pattern pt_tuple(TypeId ty, vec_pattern args);
Pattern pt_variant(TypeId ty, size_t ctor_idx, vec_pattern args);
Pattern pt_wildcard(void);

Compiler compiler_new(void);
void compiler_drop(Compiler *compiler);
TypeId type_new(Compiler *compiler, Type ty);

#endif // pm__h_
