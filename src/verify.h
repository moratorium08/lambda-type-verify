#include "ast.h"
#include "type.h"
#include <map>

enum ConstraintType {
    EQUAL_C,

};

typedef struct Constraint {
    ConstraintType type;
    Type *left;
    Type *right;
} Constraint;

int verify(char * s);
Type *infer(char *s);
Type *solve(Type *t, vector<Constraint *>constraints, vector<Type*>unknown_types);
Ast * str2ast(char *s);
void match_type(Type *left_new_t, Type *right_new_t, map<int, Type*>unknown_types);
