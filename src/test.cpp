#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<queue>
#include<cmath>
#include<map>
#include<set>
#include<string.h>
#include<cstdlib>
#include "verify.h"
using namespace std;
const int INF = 1 << 29;
const double EPS = 1e-9;

typedef long long ll;

#define SORT(c) sort((c).begin(),(c).end())
#define FOR(i,a,b) for(int i=(a);i<(b);++i)
#define rep(i,n)  FOR(i,0,n)

extern int typecmp(Type *a, Type *b);
extern Type* dfsAst(Ast *ast, vector<Variable*> globals);

void run_test(string describe, int flag, int space);

int test_passed_count = 0;
int test_count = 0;

void test_variable() {
    cout << "Variable" << endl;
    Variable *v = make_variable_by_name("v");
    run_test("make_variable_by_name", strcmp(v->name, "v") == 0, 2);

    Type *t = make_primitive("test");
    Variable *v2 = make_variable("v2", t);
    int flag = strcmp(v2->name, "v2") == 0;
    flag &= typecmp(v2->type, t);
    run_test("make_variable", flag, 2);
}

void test_type() {
    cout << "Type" << endl;
    Type *t = make_primitive("prim");
    run_test("typecmp", typecmp(t, t), 2);
    run_test("make_primitive type", t->type == VARIABLE, 2);
    run_test("make_primitive name", strcmp(t->type_name, "prim") == 0, 2);

    Type *t2 = make_primitive("to");
    Type *f = make_func_type(t, t2);
    run_test("make_func_type from", typecmp(f->from, t), 2);
    run_test("make_func_type to", typecmp(f->to, t2), 2);

    Type *ap = apply_type(f, t);
    run_test("apply_type", typecmp(ap, t2), 2);

    Type *t3 = make_type_from_str("prim->to", 0);
    run_test("make_type_from_str", typecmp(t3, f), 2);
}


int test_id() {
    Variable *x = make_variable_by_name("x");
    Ast *x_ast = make_var_ast(x);
    Ast *lambda_x_ast = make_lambda_prim_ast("x", &INT_t);
    Ast *lambda_x = make_lambda_ast(lambda_x_ast, x_ast);
    Type *int_t = make_primitive("int");
    Type *target = make_func_type(int_t, int_t);
    vector<Variable*> globals;
    Type *type = dfsAst(lambda_x, globals);

    return typecmp(type, target);
}

// lambda x: x + 1
int test_int2int() {
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
    return typecmp(type, target);
}

int test_int2int_2int2int() {
    Type *Int2Int_t = make_func_type(&INT_t, &INT_t);;
    Type *Int2Int_fn_t = make_type();
    Int2Int_fn_t->type = TYPE;
    Int2Int_fn_t->type_fn = Int2Int_t;
    Type *Int2Int_fn_2_Int_t = make_func_type(Int2Int_fn_t, Int2Int_t);

    Variable *x = make_variable_by_name("x");
    Variable *f= make_variable_by_name("f");

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

    vector<Variable*> globals;
    Type *type = dfsAst(ast, globals);
    return typecmp(type, target);
}


int test_id_from_parser() {
    vector<Variable *> globals;
    Ast *ast = str2ast("lambda y:int.y");
    Type *type = dfsAst(ast, globals);
    Type *Int2Int_t = make_func_type(&INT_t, &INT_t);;
    return typecmp(Int2Int_t, type);
}

void print_spaces(int space) {
    for (int i = 0; i < space; i++ ) {
        cout << " ";
    }
}


void run_test(string describe, int flag, int space) {
    print_spaces(space);
    cout << describe << "...";
    if (flag) {
        test_passed_count++;
        cout << "\033[32mOK\033[0m" << endl;
    }
    else {
        cout << "\033[31mNG\033[0m" << endl;
    }
    test_count++;
}

void test_overall_type_verification() {
    cout << "Over All Type Verification Check" << endl;
    try {
        int flag = test_id_from_parser();
        run_test("verify id from parser", flag, 2);

        char id[] = "id:int->int = lambda x:int.x";
        char fx[] = "f:(int->int)->int->int = lambda f:int->int.lambda x:int.f(x)";
        char fail_fx[] = "f:(int->int)->int->int = lambda f:int->str.lambda x:int.f(x)";
        int ret = verify(id);
        run_test(id, ret, 2);
        ret = verify(fx);
        run_test(fx, ret, 2);
        ret = verify(fail_fx);
        run_test(fail_fx, ret ^ 1, 2);
    }
    catch (char *str) {
        cout << "  Failed." << endl;
    }
}

void test_overall_ast_function() {
    cout << "Overall Ast Function Check" << endl;

    try {
        int flag = test_id();
        run_test("run test id matching", flag, 2);

        flag = test_int2int();
        run_test("verify lambda x:int.+(x)(1)", flag, 2);

        flag = test_int2int_2int2int();
        run_test("verify lambda f:int->int.lambda x:int.f(f(x))", flag, 2);
    }
    catch (char *str) {
        cout << "  Failed." << endl;
    }
}

void print_result() {
    cout << endl;
    if (test_passed_count == test_count) {
        cout << "\033[32mAll tests passed.\033[0m" << endl;
    } else{
        cout << "\033[31mTest Failed. \033[33m" << test_passed_count;
        cout << " / " << test_count << "\033[0m" << endl;
    }
}

int main(int argc, char * argv[])
{
    ios::sync_with_stdio(false);
    test_variable();
    test_type();
    test_overall_ast_function();
    test_overall_type_verification();
    print_result();
    return 0;
}
