#ifndef pm__h_
#define pm__h_

#include <stddef.h>

typedef enum TypeKind TypeKind;
typedef struct Type Type;
typedef struct VariantConstructor VariantConstructor;

enum TypeKind {
    TK_BOOL,
    TK_INT,
    TK_TUPLE,
    TK_VARIANT
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

extern const Type *const ty_bool;
extern const Type *const ty_int;

Type *new_tuple(const Type *const *args, size_t n);
Type *new_variant(const VariantConstructor *ctors, size_t n);

void free_type(Type *ty);

#endif // pm__h_
