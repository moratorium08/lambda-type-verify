#include <cstdlib>
#include <string>
#include <vector>
#include <regex.h>
#include <stdarg.h>
#include "verify.h"

using namespace std;


void panic(char *s) {
    printf("%s", s);
    exit(-1);
}

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

void strscpy(char *buf, int count, ...) {
    va_list ap;
    va_start(ap, count);
    int idx = 0;
    for (int i = 0; i < count; i++) {
        char *tmp = va_arg(ap, char*);
        for (int j = 0; j < strlen(tmp); j++) {
            buf[idx++] = tmp[j];
        }
    }
}


char * char_alloc(int x) {
    char *ret = (char *)malloc(x + 1);
    memset(ret, x + 1, 0);
    return ret;
}

char *type2str(Type *t) {
    if (t->type == VARIABLE) {
        char *ret = char_alloc(strlen(t->type_name));
        strcpy(ret, t->type_name);
        return ret;
    }
    else if(t->type == FUNCTION) {
        char *from = type2str(t->from);
        char *to = type2str(t->to);
        char *ret = char_alloc(strlen(from) + strlen(to) + 2);
        strscpy(ret, 3, from, "->", to);
        free(from);
        free(to);
        return ret;
    } else {
        char *tmp = type2str(t->type_fn);
        char *ret = char_alloc(strlen(tmp) + 2);
        strscpy(ret, 3, "(", tmp, ")");
        free(tmp);
        return ret;
    }
}

int typecmp(Type *a, Type *b) {
    char *s = type2str(a);
    char *t = type2str(b);
    return strcmp(s, t) == 0;
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
        _print_type(t->from);
        printf(" -> ");
        _print_type(t->to);
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
Type * _make_no_func_type_from_str(char *s) {
    static const char var_regex[] = "^ *([a-zA-Z][a-zA-Z0-9]*) *$";
    regex_t varbuf;
    if(regcomp(&varbuf, var_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    regmatch_t pm[2];
    if(regexec(&varbuf, s, 2, pm, 0) == 0)
    {
        char *name = create_substr(s, pm[1].rm_so, pm[1].rm_eo);
        Type *t = make_type();
        t->type = VARIABLE;
        t->type_name = name;
        return t;
    }
    static const char var_pr_regex[]
        = "^ *\\((.+)\\) *$";
    regex_t buf;
    if(regcomp(&buf, var_pr_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    if(regexec(&buf, s, 2, pm, 0) == 0)
    {
        char *name = create_substr(s, pm[1].rm_so, pm[1].rm_eo);
        Type *t = make_type_from_str(name);
        Type *ret = make_type();
        ret->type = TYPE;
        ret->type_fn = t;

        return ret;
    }
}
Type * make_type_from_str(char *s) {
    //static const char arrow[] = "^ *(.+?)->(.+)$";
    int l = strlen(s);
    int open_cnt = 0;
    int st = 0;
    Type *ret;
    for (int i = 0; i < l; i++) {
        if (s[i] == '-' && (i + 1) < l && s[i + 1] == '>' && open_cnt == 0) {
            char *type_str = create_substr(s, st, i);
            Type *t = _make_no_func_type_from_str(type_str);
            if (st == 0) {
                ret = t;
            }
            else {
                ret = make_func_type(ret, t);
            }
            if ((i + 2) >= l) {
                printf("Invalid syntax\n");
                exit(-1);
            }
            st = i + 2;
            i = st;
        }
        else if (s[i] == '(') {
            open_cnt++;
        }
        else if(s[i] == ')') {
            if (open_cnt <= 0) {
                printf("parenthesis is illegal\n");
                exit(-1);
            }
            open_cnt--;
        }
    }
    // TODO:
    // finally ... -> [koko]
    char *type_str = create_substr(s, st, l);
    Type *t = _make_no_func_type_from_str(type_str);
    if (st == 0) {
        ret = t;
    }
    else {
        ret = make_func_type(ret, t);
    }
    if (ret->type == FUNCTION) {
        Type *tmp = ret;
        ret = make_type();
        ret->type = TYPE;
        ret->type_fn = tmp;
    }

    return ret;
}

Ast *str2ast(char *s) {
    // lambda x:var matching
    static const char lambda_regex[]
        = "^ *lambda +([a-zA-Z][a-zA-Z0-9]*):([^.]+)\\.(.+)$";
    regex_t lambdabuf;
    if(regcomp(&lambdabuf, lambda_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    regmatch_t pm[4];
    if( regexec( &lambdabuf, s, 4, pm, 0 ) == 0 )
    {
        // var name
        int st, ed;
        char *name = create_substr(s, pm[1].rm_so, pm[1].rm_eo);
        char *type_name = create_substr(s, pm[2].rm_so, pm[2].rm_eo);

        char *exp = create_substr(s, pm[3].rm_so, pm[3].rm_eo);
        Ast *right = str2ast(exp);
        free(exp);

        Type *t = make_type_from_str(type_name);

        Ast *left = make_lambda_prim_ast(name, t);
        Ast *ret = make_lambda_ast(left, right);
        return ret;
    }

    // Apply Handling
    static const char apply_regex[] = "^ *([a-zA-Z][a-zA-Z0-9]*)\\((.*)\\)$";
    regex_t applybuf;
    if(regcomp(&applybuf, apply_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    regmatch_t applypm[4];
    if(regexec(&applybuf, s, 4, applypm, 0) == 0)
    {
        char *name = create_substr(s, applypm[1].rm_so, applypm[1].rm_eo);
        char *exp = create_substr(s, applypm[2].rm_so, applypm[2].rm_eo);

        Ast *right = str2ast(exp);
        free(exp);

        Variable *func = make_variable_by_name(name);
        Ast *left = make_var_ast(func);
        Ast *ret = make_apply_ast(left, right);
        return ret;
    }

    // Variable Handling
    static const char var_regex[] = "^ *[A-Za-z][A-Za-z]*$";
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

int find_first_split_point(char *s, char split_c) {
    int l = strlen(s);
    int i;
    for (i = 0; i < l; i++)
        if (split_c == s[i])
            break;
    return i;
}

int verify(char* s) {
    printf("-----debug-----\n");
    int l = strlen(s);
    // TODO: Semantic Parsing
    // TODO: Type Inference

    int split_point = find_first_split_point(s, '=');
    char * lambda_str = create_substr(s, split_point + 1, l);
    char *let_str = create_substr(s, 0, split_point);

    int split_point_2 = find_first_split_point(let_str, ':');
    char *type_str = create_substr(let_str, split_point_2 + 1, split_point);
    char *func_name = create_substr(let_str, 0, split_point_2);
    free(let_str);

    Ast *ast = str2ast(lambda_str);
    print_ast(ast);

    /** Global Variable Settings **/
    vector<Variable*> globals;

    Type *type = dfsAst(ast, globals);
    print_type(type);
    printf("--\n");

    Type *target = make_type_from_str(type_str);
    print_type(target);

    //Type *target = make_func_type(&INT_t, &INT_t);;
    int result = typecmp(type, target);
    // TODO: Free
    printf("-----debug-----\n");

    return result;
}
