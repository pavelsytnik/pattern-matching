/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef pm__h_
#define pm__h_

#include <stddef.h>
#include <stdint.h>

typedef enum TypeKind TypeKind;
typedef enum ConstructorKind ConstructorKind;
typedef enum PatternKind PatternKind;

typedef struct Type Type;
typedef struct VariantConstructor VariantConstructor;
typedef struct Constructor Constructor;
typedef struct Pattern Pattern;

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
    CK_VARIANT_CTOR
};

enum PatternKind {
    PK_CTOR,
    PK_WILDCARD
};

struct VariantConstructor {
    char *name;
    const Type **args;
    size_t args_n;
};

struct Type {
    TypeKind kind;

    union {
        struct {
            const Type **args;
            size_t args_n;
        } tuple;

        struct {
            VariantConstructor *ctors;
            size_t ctors_n;
        } variant;
    };
};

struct Constructor {
    ConstructorKind kind;

    union {
        int32_t integer;
        struct {
            const Type *const *args;
            size_t args_n;
        } tuple;
        const VariantConstructor *variant_ctor;
    };
};

struct Pattern {
    PatternKind kind;

    union {
        struct {
            Constructor ctor;
            Pattern *args;
            size_t args_n;
        } ctor;
    };
};

extern const Type *const ty_bool;
extern const Type *const ty_int;

Type *new_tuple(const Type *const *args, size_t n);
Type *new_variant(const VariantConstructor *ctors, size_t n);

void free_type(Type *ty);

Pattern pt_false(void);
Pattern pt_true(void);
Pattern pt_int(int32_t i);
Pattern pt_tuple(const Type *ty, Pattern *args);
Pattern pt_variant(const Type *ty, const char *name, Pattern *args);
Pattern pt_wildcard(void);

#endif // pm__h_
