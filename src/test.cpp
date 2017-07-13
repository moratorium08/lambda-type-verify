#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<queue>
#include<cmath>
#include<map>
#include<set>
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

int test_id() {
    Type INT2INT_t;
    INT2INT_t.type = FUNCTION;
    INT2INT_t.from = &INT_t;
    INT2INT_t.to = &INT_t;

    /*** Making Variables ***/
    Variable x, const1, add;
    x.name = "x";

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

    Type *target = &INT2INT_t;

    // make_lambda should have int->int

    vector<Variable*> globals;
    Type *type = dfsAst(&ast, globals);
    // print_type(type);

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

void run_test(string describe, int flag) {
    cout << describe << "...";
    if (flag) {
        cout << "OK" << endl;
    }
    else {
        cout << "NG" << endl;
    }
}

int main(int argc, char * argv[])
{
    ios::sync_with_stdio(false);
    int flag = test_id();
    run_test("run test id matching", flag);

    flag = test_int2int();
    run_test("verify lambda x:int. x + 1", flag);

    flag = test_int2int_2int2int();
    run_test("verify lambda f:int->int.lambda x:int.f(f(x))", flag);
    return 0;
}
