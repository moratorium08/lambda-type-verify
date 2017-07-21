#ifndef AST_H
#define AST_H
#include "type.h"

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
Variable *make_var();
Ast *make_var_ast(Variable *v);
Lambda *make_lambda(Variable *var);
Ast *make_lambda_prim_ast(char *var_name, Type *var_type);
Ast *make_lambda_prim_ast_unknown(char *var_name);
Ast *make_apply_ast(Ast *left, Ast *right);
Ast *make_lambda_ast(Ast *lambda, Ast *right);
Type * handle_lambda(Ast *ast, vector<Variable*> *globals);
Type * get_type (Variable *val, vector<Variable*> variables);


void print_ast(Ast *ast);
void print_var(Variable *var);
#endif
