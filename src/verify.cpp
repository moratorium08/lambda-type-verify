#include <cstdlib>
#include <string>
#include <vector>
#include "verify.h"

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

/* lambda expression rule
 *
 * let f:int -> int = (lambda: f: int -> int. lambda y:int. f(f(y)) (lambda z:int. + z 1)
 *
 * let [func_name]:[type] = (expressions)
 * lambda [name]:[type].(expressions)
 *
 */
/*
 * let f:int->int = lambda x:int. +(x)(1)
 * -> ok
 */


Type * get_type (Variable *val, vector<Variable*> variables) {
    for(auto itr = variables.begin(); itr != variables.end(); ++itr) {
        if(strcmp((*itr)->name, val->name) == 0) {
            return (*itr)->type;
        }
    }
    return NULL;
}


int typecmp(Type *a, Type *b) {
    if (a->type == VARIABLE && b->type == VARIABLE
            && strcmp(a->type_name, b->type_name) == 0) {
        return 1;
    }

    if (a->type == FUNCTION && b->type == FUNCTION) {
        int from_flag = typecmp(a->from, b->from);
        int to_flag = typecmp(a->to, b->to);
        return from_flag & to_flag;
    }
    else if (a->type == TYPE && b->type == TYPE) {
        return typecmp(a->type_fn, b->type_fn);
    }
    else {
        return 0;
    }
}


void _print_type(Type*t){
    if (t->type == VARIABLE) {
        printf("%s", t->type_name);
    }
    else if(t->type == FUNCTION) {
        //printf("(");
        _print_type(t->from);
        //printf(")->(");
        printf(" -> ");
        _print_type(t->to);
        //printf(")");
    } else {
        printf("(");
        _print_type(t->type_fn);
        printf(")");
    }
}


void print_type(Type *t) {
    _print_type(t);
    printf("\n");
}


Type * get_first_arg(Type *func) {
    if (func->type != FUNCTION) {
        return func;
    }
    return get_first_arg(func->from);
}


Type * _apply_type(Type *left_t, Type *right_t) {
    if (left_t->type != FUNCTION) {
        printf("There is a mismatch of applying variables to functions\n");
        exit(-1);
    }
    int flag = typecmp(left_t->from, right_t);
    if (flag) {
        Type *ret = left_t->to;
        //free(left_t);
        return ret;
    }
    return make_func_type(_apply_type(left_t->from, right_t), left_t->to);
}


Type * apply_type(Type *left_t, Type *right_t) {
    if (left_t->type != FUNCTION) {
        printf("You cannot apply variable to variable\n");
        exit(-1);
    }
    return _apply_type(left_t, right_t);
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


Type* dfsAst(Ast *ast, vector<Variable*>globals) {
    if (ast->type == VARIABLE_AST) {
        if (!(ast->val->type)) {
            Type *t = get_type(ast->val, globals);
            if (!t) {
                printf("Cannot find variable %s\n", ast->val->name);
                exit(-1);
            }
            else {
                return t;
            }
        }
        else {
            return ast->val->type;
        }
    }

    if (ast->type == APPLY_AST) {
        Type *left_t = dfsAst(ast->left, globals);
        Type *right_t = dfsAst(ast->right, globals);

        Type *result_t = apply_type(left_t, right_t);
        return result_t;
    }

    if (ast->type == LAMBDA_AST) {
        Type *left_t = handle_lambda(ast->left, &globals);
        Type *right_t = dfsAst(ast->right, globals);
        return make_func_type(left_t, right_t);
    }

    printf("ast type error");
    exit(-1);
}

Variable * make_variable_by_name(char *s) {
    Variable *x = (Variable*)malloc(sizeof(Variable));
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
    ast->type = VARIABLE_AST;
    ast->val = v;
    return ast;
}

Lambda *make_lambda(Variable *var) {
    Lambda *lambda = (Lambda *)malloc(sizeof(Lambda));
    lambda->arg = var;
    return lambda;
}

Ast *make_lambda_prim_ast(char *var_name, Type *var_type) {
    Variable *var = make_variable(var_name, var_type);
    Lambda *lambda = make_lambda(var);
    Ast * ast = (Ast *)malloc(sizeof(Ast));
    ast->type = LAMBDA_PRIM_AST;
    ast->lambda = lambda;
    return ast;
}

Ast *make_apply_ast(Ast *left, Ast *right) {
    Ast *ast = (Ast *)malloc(sizeof(Ast));
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

int verify(char* s) {
    int l = strlen(s);

    /*** Define Types ***/
    Type *INT2INT_t = make_func_type(&INT_t, &INT_t);;
    Type *INT2INT2INT_t = make_func_type(INT2INT_t, &INT_t);

    /*** Making Variables ***/
    Variable *x = make_variable_by_name("x");
    Variable *const1 = make_variable_by_name("!1");
    Variable *add = make_variable_by_name("+");

    /*** Built ASTs ***/
    Ast *const1_ast = make_var_ast(const1);
    Ast *x_ast = make_var_ast(x);
    Ast *add_ast = make_var_ast(add);
    Ast *lambda_x_ast = make_lambda_prim_ast("x", &INT_t);
    Ast *applyx = make_apply_ast(add_ast, x_ast);
    Ast *apply1 = make_apply_ast(applyx, const1_ast);
    Ast *lambda_x = make_lambda_ast(lambda_x_ast, apply1);

    Ast *ast = lambda_x;  // target ast
    Type *target = INT2INT_t;  // make_lambda should have int->int

    /** Global Variable Settings **/
    Variable *const1_g = make_variable("!1", &INT_t);
    Variable *add_g= make_variable("+", INT2INT2INT_t);
    vector<Variable*> globals;
    globals.push_back(add_g);
    globals.push_back(const1_g);

    Type *type = dfsAst(ast, globals);
    print_type(type);

    return typecmp(type, target);
}
