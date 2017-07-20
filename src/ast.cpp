#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>
#include "ast.h"
#include "util.h"

using namespace std;

Type * make_primitive(char * type_name) {
    Type *type = (Type*)malloc(sizeof(Type));
    type->type = VARIABLE;
    type->type_name = type_name;
    return type;
}

Type * make_func_type(Type *from, Type *to) {
    Type *ret = (Type*)malloc(sizeof(Type));
    ret->type = FUNCTION;
    ret->from = from;
    ret->to = to;
    return ret;
}

void print_var(Variable *var) {
    printf("%s:", var->name);
    print_type(var->type);
}
void _print_ast(Ast *ast, int space) {
    switch(ast->type) {
        case VARIABLE_AST:
            _print_spaces(space);
            printf("[var]%s\n", ast->val->name);
            break;
        case APPLY_AST:
            _print_spaces(space);
            _print_ast(ast->left, 2);
            _print_ast(ast->right, space + 2);
            break;
        case LAMBDA_AST:
            _print_spaces(space);
            _print_ast(ast->left, 2);
            _print_ast(ast->right, space + 2);
            break;
        case LAMBDA_PRIM_AST:
            _print_spaces(space);
            printf("lambda %s:", ast->lambda->arg->name);
            print_type(ast->lambda->arg->type);
            break;
    }
}

void print_ast(Ast *ast) {
    _print_ast(ast, 0);
}

Variable *make_var() {
    Variable * v = (Variable*)malloc(sizeof(Variable));
    memset(v, 0, sizeof(Variable));
    return v;
}

Variable * make_variable_by_name(char *s) {
    Variable *x = make_var();
    x->name = s;
    return x;
}

Variable *make_variable(char *name, Type *type) {
    Variable *var = make_variable_by_name(name);
    var->type = type;
    return var;
}

Ast *make_var_ast(Variable *v){
    Ast *ast = (Ast *)malloc(sizeof(Ast));
    memset(ast, 0, sizeof(Ast));
    ast->type = VARIABLE_AST;
    ast->val = v;
    return ast;
}

Lambda *make_lambda(Variable *var) {
    Lambda *lambda = (Lambda *)malloc(sizeof(Lambda));
    memset(lambda, 0, sizeof(Lambda));
    lambda->arg = var;
    return lambda;
}

Ast *make_lambda_prim_ast(char *var_name, Type *var_type) {
    Variable *var = make_variable(var_name, var_type);
    Lambda *lambda = make_lambda(var);
    Ast * ast = (Ast *)malloc(sizeof(Ast));
    memset(ast, 0, sizeof(Ast));
    ast->type = LAMBDA_PRIM_AST;
    ast->lambda = lambda;
    return ast;
}

Ast *make_apply_ast(Ast *left, Ast *right) {
    Ast *ast = (Ast *)malloc(sizeof(Ast));
    memset(ast, 0, sizeof(Ast));
    ast->type = APPLY_AST;
    ast->left = left;
    ast->right = right;
    return ast;
}

Ast *make_lambda_ast(Ast *lambda, Ast *right) {
    if (lambda->type != LAMBDA_PRIM_AST) {
        printf("illegal lambda\n");
        exit(-1);
    }
    Ast *ast = (Ast *)malloc(sizeof(Ast));
    ast->type = LAMBDA_AST;
    ast->left = lambda;
    ast->right = right;
    return ast;
}

Type * handle_lambda(Ast *ast, vector<Variable*> *globals) {
    if (ast->type != LAMBDA_PRIM_AST) {
        printf("Invalid expression.\n");
        exit(-1);
    }
    Lambda *lambda = ast->lambda;
    Type *ret = lambda->arg->type;
    globals->push_back(lambda->arg);
    return ret;
}

Type * get_type (Variable *val, vector<Variable*> variables) {
    for(auto itr = variables.begin(); itr != variables.end(); ++itr) {
        if(strcmp((*itr)->name, val->name) == 0) {
            return (*itr)->type;
        }
    }
    return NULL;
}
