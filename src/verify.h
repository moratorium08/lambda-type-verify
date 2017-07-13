#include <string>
using namespace std;

enum {
    FUNCTION,
    VARIABLE,
    TYPE
};

typedef struct Type {
    int type;
    union {
        char* type_name;
        Type* type_fn;
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

// Ast related functions
Variable * make_variable_by_name(char *s);
Variable *make_variable(char *name, Type *type);
Ast *make_var_ast(Variable *v);
Lambda *make_lambda(Variable *var);
Ast *make_lambda_prim_ast(char *var_name, Type *var_type);
Ast *make_apply_ast(Ast *left, Ast *right);
Ast *make_lambda_ast(Ast *lambda, Ast *right);

// Type related functions
Type *make_type();
Type * apply_type(Type *left_t, Type *right_t);
void print_type(Type *t);
int typecmp(Type *a, Type *b);
Type * make_func_type(Type *from, Type *to);
Type * make_primitive(char * type_name);

int verify(char * s);

//typedef string Type;
static char * primitive_names[] = {"int", "string", "float", "bool", "char", "unit"};
static Type INT_t = {VARIABLE, primitive_names[0]};
static Type STRING_t = {VARIABLE, primitive_names[1]};
static Type FLOAT_t =  {VARIABLE, primitive_names[2]};
static Type BOOL_t  = {VARIABLE, primitive_names[3]};
static Type CHAR_t  = {VARIABLE, primitive_names[4]};
static Type UNIT_t  = {VARIABLE, primitive_names[5]};
