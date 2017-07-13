#include <cstdlib>
#include <string>
#include <vector>
#include "verify.h"

using namespace std;

//typedef string Type;

Type * make_primitive(char * type_name) {
    Type *type = (Type*)malloc(sizeof(Type));
    type->type = VARIABLE;
    type->type_name = type_name;
    return type;
}
char * primitive_names[] = {"int", "string", "float", "bool", "char", "unit"};
Type INT_t = {VARIABLE, primitive_names[0]};
Type STRING_t = {VARIABLE, primitive_names[1]};
Type FLOAT_t =  {VARIABLE, primitive_names[2]};
Type BOOL_t  = {VARIABLE, primitive_names[3]};
Type CHAR_t  = {VARIABLE, primitive_names[4]};
Type UNIT_t  = {VARIABLE, primitive_names[5]};

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
    else {
        return 0;
    }
}

void _print_type(Type*t){
    if (t->type == VARIABLE) {
        printf("%s", t->type_name);
    }
    else {
        printf("(");
        _print_type(t->from);
        printf(")->(");
        _print_type(t->to);
        printf(")");
    }
}
void print_type(Type *t) {
    _print_type(t);
    printf("\n");
}

Type * apply_type(Type *left_t, Type *right_t) {
    if (left_t->type != FUNCTION) {
        printf("You cannot apply variable to variable\n");
        exit(-1);
    }
    int flag = typecmp(left_t->from, right_t);
    if (flag) {
        return left_t->to;
    }

    printf("There is a mismatch of applying variables to functions\n");
    print_type(left_t);
    print_type(right_t);
    exit(-1);
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

Type * make_func_type(Type *from, Type *to) {
    Type *ret = (Type*)malloc(sizeof(Type));
    ret->type = FUNCTION;
    ret->from = from;
    ret->to = to;
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

int verify(char* s) {
    int l = strlen(s);

    /*** Define Types ***/
    Type INT2INT_t;
    INT2INT_t.type = FUNCTION;
    INT2INT_t.from = &INT_t;
    INT2INT_t.to = &INT_t;

    Type INT2INT2INT_t;
    INT2INT2INT_t.type = FUNCTION;
    INT2INT2INT_t.from = &INT2INT_t;
    INT2INT2INT_t.to = &INT_t;

    /*** Making Variables ***/
    Variable x, const1, add;
    x.name = "x";
    const1.name = "!1"; //Assume that there is no variable starts with !
    add.name = "+";

    /*** Built ASTs ***/
    Ast x_ast, lambda_x_ast, ast;
    x_ast.type = VARIABLE_AST;
    lambda_x_ast.type = LAMBDA_PRIM_AST;
    x_ast.val = &x;
    Lambda lambda;
    Variable x_g;
    x_g.name = "x";
    x_g.type = &INT_t;
    lambda.arg = &x_g;
    lambda_x_ast.lambda = &lambda;
    ast.type = LAMBDA_AST;
    ast.left = &lambda_x_ast;
    ast.right = &x_ast;

    /*
    Ast const1_ast, x_ast, add_ast, lambda_x_ast;

    const1_ast.type = VARIABLE_AST;
    x_ast.type = VARIABLE_AST;
    add_ast.type = VARIABLE_AST;
    lambda_x_ast.type = LAMBDA_PRIM_AST;
    const1_ast.val = &const1;
    x_ast.val = &x;
    add_ast.val = &add;

    Lambda lambda;
    Variable x_g;
    x_g.name = "x";
    x_g.type = &INT_t;
    lambda.arg = &x_g;
    lambda_x_ast.lambda = &lambda;

    Ast applyx, apply1, make_lambda;
    applyx.type = APPLY_AST;
    applyx.left = &add_ast;
    applyx.right= &x_ast;

    apply1.type = APPLY_AST;
    apply1.left = &applyx;
    apply1.right = &const1_ast;

    make_lambda.type = LAMBDA_AST;
    make_lambda.left = &lambda_x_ast;
    make_lambda.right = &apply1;

    Ast ast; //target ast
    ast = make_lambda;*/

    // make_lambda should have int->int

    /** Global Variable Settings **/
    Variable const1_g, add_g;
    const1_g.name = "const1";
    const1_g.type = &INT_t;
    add_g.name = "+";
    add_g.type = &INT2INT2INT_t;

    vector<Variable*> globals;
    globals.push_back(&add_g);
    globals.push_back(&const1_g);
    Type *type = dfsAst(&ast, globals);
    print_type(type);

    return 0;
}
