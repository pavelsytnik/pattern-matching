/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef pm__h_
#define pm__h_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "util/types.h"

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

x_declare_vec(vec_type, Type);
x_declare_vec(vec_typeid, TypeId);
x_declare_vec(vec_variant, VariantConstructor);
x_declare_vec(vec_ctor, Constructor);
x_declare_vec(vec_pattern, Pattern);
x_declare_vec(vec_column, Column);
x_declare_vec(vec_row, Row);
x_declare_vec(vec_case, Case);

enum TypeKind {
    TK_BOOL,
    TK_INT,
    TK_TUPLE,
    TK_VARIANT
};

struct Type {
    TypeKind kind;

    union {
        vec_typeid tuple;
        vec_variant variant;
    };
};

Type ty_bool(void);
Type ty_int(void);
Type ty_tuple(vec_typeid args);
Type ty_variant(vec_variant ctors);

void ty_drop(Type *ty);

#define t_value_drop ty_drop
#define T vec_type, Type, (x_declared)
#include "util/vec.h"

#define T vec_typeid, TypeId, (x_declared)
#include "util/vec.h"

struct VariantConstructor {
    char *name;
    vec_typeid args;
};

VariantConstructor variant_new(const char *name, vec_typeid args);
void variant_drop(VariantConstructor *ctor);

#define t_value_drop variant_drop
#define T vec_variant, VariantConstructor, (x_declared)
#include "util/vec.h"

enum ConstructorKind {
    CK_FALSE,
    CK_TRUE,
    CK_INT,
    CK_TUPLE,
    CK_VARIANT
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

Constructor cs_false(void);
Constructor cs_true(void);
Constructor cs_int(int32_t i);
Constructor cs_tuple(TypeId ty);
Constructor cs_variant(TypeId ty, size_t ctor_idx);

bool cs_eq(const Constructor *c1, const Constructor *c2);

#define T vec_ctor, Constructor, (x_declared)
#include "util/vec.h"

enum PatternKind {
    PK_CTOR,
    PK_WILDCARD
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

Pattern pt_false(void);
Pattern pt_true(void);
Pattern pt_int(int32_t i);
Pattern pt_tuple(TypeId ty, vec_pattern args);
Pattern pt_variant(TypeId ty, size_t ctor_idx, vec_pattern args);
Pattern pt_wildcard(void);

Pattern pt_clone(Pattern pt);
void pt_drop(Pattern *pt);

#define t_value_clone pt_clone
#define t_value_drop pt_drop
#define T vec_pattern, Pattern, (x_declared | x_use_clone)
#include "util/vec.h"

struct Column {
    TypeId type;
    Pattern pattern;
};

Column column_new(TypeId ty, Pattern pt);
Column column_clone(Column column);
void column_drop(Column *col);

#define t_value_drop column_drop
#define T vec_column, Column, (x_declared)
#include "util/vec.h"

struct Row {
    vec_column columns;
    Body body;
};

Row row_new(vec_column cols, Body body);
void row_drop(Row *row);

#define t_value_drop row_drop
#define T vec_row, Row, (x_declared)
#include "util/vec.h"

enum DecisionKind {
    DK_LEAF,
    DK_FAILURE,
    DK_BRANCH
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

Decision dc_leaf(Body body);
Decision dc_failure(void);
Decision dc_branch(vec_case cases, Decision *fallback);

void decision_drop(Decision *decision);

struct Case {
    Constructor ctor;
    Decision decision;
};

Case case_new(Constructor ctor, Decision decision);
void case_drop(Case *kase);

#define t_value_drop case_drop
#define T vec_case, Case, (x_declared)
#include "util/vec.h"

struct Compiler {
    vec_type types;
};

Compiler compiler_new(void);
void compiler_drop(Compiler *compiler);

TypeId new_type(Compiler *compiler, Type ty);
const Type *get_type(const Compiler *compiler, TypeId id);

Decision compile(const Compiler *compiler, vec_row rows);

#endif // pm__h_
