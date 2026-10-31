/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef pm__h_
#define pm__h_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "template/types.h"

typedef enum TypeKind TypeKind;
typedef enum ConstructorKind ConstructorKind;
typedef enum PatternKind PatternKind;
typedef enum DecisionKind DecisionKind;

typedef struct Type Type;
typedef struct VariantConstructor VariantConstructor;
typedef struct Constructor Constructor;
typedef struct Pattern Pattern;
typedef struct Column Column;
typedef struct Row Row;
typedef struct Compiler Compiler;
typedef struct Decision Decision;
typedef struct Case Case;

typedef size_t TypeId;
typedef size_t Body;

DECLARE_VEC(vec_type, Type);
DECLARE_VEC(vec_typeid, TypeId);
DECLARE_VEC(vec_variant, VariantConstructor);
DECLARE_VEC(vec_ctor, Constructor);
DECLARE_VEC(vec_pattern, Pattern);
DECLARE_VEC(vec_column, Column);
DECLARE_VEC(vec_row, Row);
DECLARE_VEC(vec_case, Case);

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

enum DecisionKind {
    DK_LEAF,
    DK_FAILURE,
    DK_BRANCH
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

struct Column {
    TypeId type;
    Pattern pattern;
};

struct Row {
    vec_column columns;
    Body body;
};

struct Compiler {
    vec_type types;
};

struct Decision {
    DecisionKind kind;

    union {
        Body leaf;
        struct {
            vec_case cases;
            Decision *fallback;
        } branch;
    };
};

struct Case {
    Constructor ctor;
    Decision decision;
};

void ty_drop(Type *ty);

#define DECLARED
#define value_free ty_drop
#define T vec_type, Type
#include "template/vec.h"

#define DECLARED
#define T vec_typeid, TypeId
#include "template/vec.h"

VariantConstructor variant_from(VariantConstructor ctor);
void variant_drop(VariantConstructor *ctor);

#define DECLARED
#define value_from variant_from
#define value_free variant_drop
#define T vec_variant, VariantConstructor
#include "template/vec.h"

#define DECLARED
#define T vec_ctor, Constructor
#include "template/vec.h"

Pattern pt_clone(Pattern pt);
void pt_drop(Pattern *pt);

#define DECLARED
#define value_clone pt_clone
#define value_free pt_drop
#define T vec_pattern, Pattern
#include "template/vec.h"

Column column_clone(Column column);
void column_drop(Column *col);

#define DECLARED
#define value_clone column_clone
#define value_free column_drop
#define T vec_column, Column
#include "template/vec.h"

void row_drop(Row *row);

#define DECLARED
#define value_free row_drop
#define T vec_row, Row
#include "template/vec.h"

void case_drop(Case *kase);

#define DECLARED
#define value_free case_drop
#define T vec_case, Case
#include "template/vec.h"

void decision_drop(Decision *decision);

Type ty_bool(void);
Type ty_int(void);
Type ty_tuple(vec_typeid args);
Type ty_variant(vec_variant ctors);

Constructor cs_false(void);
Constructor cs_true(void);
Constructor cs_int(int32_t i);
Constructor cs_tuple(TypeId ty);
Constructor cs_variant(TypeId ty, size_t ctor_idx);

bool cs_eq(const Constructor *c1, const Constructor *c2);

Pattern pt_false(void);
Pattern pt_true(void);
Pattern pt_int(int32_t i);
Pattern pt_tuple(TypeId ty, vec_pattern args);
Pattern pt_variant(TypeId ty, size_t ctor_idx, vec_pattern args);
Pattern pt_wildcard(void);

Column column_new(TypeId ty, Pattern pt);
Row row_new(vec_column cols, Body body);
Case case_new(Constructor ctor, Decision decision);

Decision dc_leaf(Body body);
Decision dc_failure(void);
Decision dc_branch(vec_case cases, Decision *fallback);

Compiler compiler_new(void);
void compiler_drop(Compiler *compiler);
TypeId new_type(Compiler *compiler, Type ty);
const Type *get_type(const Compiler *compiler, TypeId id);

Decision compile(const Compiler *compiler, vec_row rows);

#endif // pm__h_
