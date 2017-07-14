#include <cstdlib>
#include <string>
#include <vector>
#include <regex.h>
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


void _print_spaces(int space) {
    for (int i = 0 ; i < space; i++) {
        printf(" ");
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
    if (left_t->type == TYPE) {
        return apply_type(left_t->type_fn, right_t);
    }
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
            printf("hel\n");
            printf("p%lx\n", (unsigned long)(ast->val->type));
            print_type(ast->val->type);
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

Variable *make_var() {
    Variable * v = (Variable*)malloc(sizeof(Variable));
    memset(v, 0, sizeof(Variable));
    return v;
}

Type *make_type() {
    Type *t =  (Type*)malloc(sizeof(Type));
    memset(t, 0, sizeof(Type));
    return t;
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

// This parser only accepts the expression starts with lambda.
// [Lambda Expressions]
// lambda [name]:[type].(expressions)
// [Functional Apprecations]
// [func name](expressions)
//
// The rules of this language are only the above rules.
//

char *create_substr(char *s, int st, int ed) {
    char *ret = (char *)malloc(ed - st + 1);
    for (int i = st; i < ed; i++) {
        ret[i - st] = s[i];
    }
    ret[ed - st] = 0;
    return ret;
}

Ast *str2ast(char *s) {
    // lambda x:var matching
    static const char lambda_regex[]
        = "^lambda +([a-zA-Z][a-zA-Z0-9]*):([a-zA-Z][a-zA-Z0-9]*)\\.(.+)$";
    regex_t regexBufferLambda;
    if(regcomp(&regexBufferLambda, lambda_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    regmatch_t pm[4];
    if( regexec( &regexBufferLambda, s, 4, pm, 0 ) == 0 )
    {
        // var name
        int st, ed;
        char *name = create_substr(s, pm[1].rm_so, pm[1].rm_eo);
        char *type_name = create_substr(s, pm[2].rm_so, pm[2].rm_eo);

        char *exp = create_substr(s, pm[3].rm_so, pm[3].rm_eo);
        Ast *right = str2ast(exp);
        free(exp);

        Type *t = make_type();
        t->type = VARIABLE;
        t->type_name = type_name;

        Ast *left = make_lambda_prim_ast(name, t);
        Ast *ret = make_lambda_ast(left, right);
        return ret;
    }

    static const char apply_regex[] = "^([a-zA-Z][a-zA-Z0-9]*)\\((.*)\\)$";
    regex_t regexBufferApply;
    if(regcomp(&regexBufferApply, lambda_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    regmatch_t applypm[4];
    if(regexec(&regexBufferApply, s, 4, applypm, 0) == 0)
    {
        // toridashi syori
        printf("[!]%s\n", s);
    }

    static const char var_regex[] = "[A-Za-z][A-Za-z]*";
    regex_t varbuf;
    if (regcomp(&varbuf, var_regex, REG_EXTENDED | REG_NEWLINE) != 0) {
        printf("regex error...!\n");
        exit(-1);
    }
    regmatch_t varpm[1];
    if (regexec(&varbuf, s, 1, varpm, 0) == 0) {
        char *name = create_substr(s, varpm[0].rm_so, varpm[0].rm_eo);
        Variable *x = make_variable_by_name(name);
        Ast *ast = make_var_ast(x);
        return ast;
    }
    printf("Illegal Grammar\n");
    exit(-1);
}

int verify(char* s) {
    int l = strlen(s);
    // TODO: Semantic Parsing
    // TODO: Type Inference

    /*** Define Types ***/
    Type *Int2Int_t = make_func_type(&INT_t, &INT_t);;
    Type *Int2Int_fn_t = make_type();
    Int2Int_fn_t->type = TYPE;
    Int2Int_fn_t->type_fn = Int2Int_t;
    Type *Int2Int_fn_2_Int_t = make_func_type(Int2Int_fn_t, Int2Int_t);

    /*** Making Variables ***/
    Variable *x = make_variable_by_name("x");
    Variable *f= make_variable_by_name("f");

    /*** Built ASTs ***/
    Ast *f1_ast = make_var_ast(f);
    Ast *x_ast = make_var_ast(x);
    Ast *f2_ast = make_var_ast(f);

    Ast *lambda_x_ast = make_lambda_prim_ast("x", &INT_t);
    Ast *lambda_f_ast = make_lambda_prim_ast("f", Int2Int_fn_t);

    Ast *applyx = make_apply_ast(f1_ast, x_ast);
    Ast *applyfx = make_apply_ast(f2_ast, applyx);
    Ast *lambda_x = make_lambda_ast(lambda_x_ast, applyfx);
    Ast *lambda_f = make_lambda_ast(lambda_f_ast, lambda_x);

    Ast *ast = lambda_f;  // target ast
    Type *target = Int2Int_fn_2_Int_t;  // make_lambda should have int->int
    print_ast(ast);

    /** Global Variable Settings **/
    vector<Variable*> globals;

    Type *type = dfsAst(ast, globals);
    print_type(type);
    int result = typecmp(type, target);
    // TODO: Free

    return result;
}
