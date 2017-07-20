#include <cstdlib>
#include <vector>
#include <regex.h>
#include "verify.h"
#include "type.h"
#include "ast.h"
#include "util.h"

using namespace std;

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

Type *make_type() {
    Type *t =  (Type*)malloc(sizeof(Type));
    memset(t, 0, sizeof(Type));
    return t;
}


// This parser only accepts the expression starts with lambda.
// [Lambda Expressions]
// lambda [name]:[type].(expressions)
// [Functional Apprecations]
// [func name](expressions)
//
// The rules of this language are only the above rules.
//


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
        Type *t = make_type_from_str(name, 0);
        Type *ret = make_type();
        ret->type = TYPE;
        ret->type_fn = t;
        return ret;
    }
}

Type * make_type_from_str(char *s, int type_wrap_if_func) {
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
            i = st - 1;
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
    if (ret->type == FUNCTION && type_wrap_if_func==1) {
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
        char *name = create_substr(s, pm[1].rm_so, pm[1].rm_eo);
        char *type_name = create_substr(s, pm[2].rm_so, pm[2].rm_eo);
        char *exp = create_substr(s, pm[3].rm_so, pm[3].rm_eo);
        Ast *right = str2ast(exp);
        free(exp);

        Type *t = make_type_from_str(type_name, 1);

        Ast *left = make_lambda_prim_ast(name, t);
        Ast *ret = make_lambda_ast(left, right);
        return ret;
    }

    // Apply Handling
    static const char apply_regex[] = "^ *(.+)\\((.*)\\)$";
    regex_t applybuf;
    if(regcomp(&applybuf, apply_regex, REG_EXTENDED | REG_NEWLINE ) != 0 )
    {
        printf("regex error..!\n");
        exit(-1);
    }
    regmatch_t applypm[4];
    if(regexec(&applybuf, s, 4, applypm, 0) == 0)
    {
        char *func_str = create_substr(s, applypm[1].rm_so, applypm[1].rm_eo);
        char *exp = create_substr(s, applypm[2].rm_so, applypm[2].rm_eo);

        Ast *right = str2ast(exp);
        free(exp);

        Ast *left = str2ast(func_str);
        Ast *ret = make_apply_ast(left, right);
        return ret;
    }

    // Variable Handling
    static const char var_regex[] = "^ *([A-Za-z][A-Za-z0-9]*)$";
    regex_t varbuf;
    if (regcomp(&varbuf, var_regex, REG_EXTENDED | REG_NEWLINE) != 0) {
        printf("regex error...!\n");
        exit(-1);
    }
    regmatch_t varpm[2];
    if (regexec(&varbuf, s, 2, varpm, 0) == 0) {
        char *name = create_substr(s, varpm[1].rm_so, varpm[1].rm_eo);
        Variable *x = make_variable_by_name(name);
        Ast *ast = make_var_ast(x);
        return ast;
    }

    // Variable Handling
    static const char par_regex[] = "^ *\\((.+)\\) *$";
    regex_t parbuf;
    if (regcomp(&parbuf, par_regex, REG_EXTENDED | REG_NEWLINE) != 0) {
        printf("regex error...!\n");
        exit(-1);
    }
    regmatch_t parpm[2];
    if (regexec(&parbuf, s, 2, parpm, 0) == 0) {
        char *str_in_par = create_substr(s, parpm[1].rm_so, parpm[1].rm_eo);
        Ast *ast = str2ast(str_in_par);
        return ast;
    }
    printf("Illegal Grammar\n");
    exit(-1);
}

int verify(char* s) {
    // printf("-----debug-----\n");
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

    /** Global Variable Settings **/
    vector<Variable*> globals;

    Type *type = dfsAst(ast, globals);
    // print_type(type);

    Type *target = make_type_from_str(type_str, 0);
    // print_type(target);

    //Type *target = make_func_type(&INT_t, &INT_t);;
    int result = typecmp(type, target);
    // TODO: Free
    // printf("-----debug-----\n");

    return result;
}
