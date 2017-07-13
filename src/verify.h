#include <string>
using namespace std;
int verify(char * s);

enum {
    FUNCTION,
    VARIABLE
};

typedef struct Type {
    int type;
    union {
        char* type_name;
        struct {
            Type *from;
            Type *to;
        };
    };
} Type;

typedef struct Variable {
    char* name;
    Type *type;
} Variable;

typedef struct Lambda {
    Variable *arg;
    Type *from;
    Type *to;
} Lambda;

enum {
    VARIABLE_AST,
    APPLY_AST,
    LAMBDA_AST,
    LAMBDA_PRIM_AST,
};

typedef struct Ast {
    int type;
    union {
        Variable *val;
        Lambda *lambda;
        struct {
            Ast *left;
            Ast *right;
        };
    };
} Ast;
