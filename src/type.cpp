#include <cstdlib>
#include "ast.h"
#include "util.h"
#include "type.h"


char *type2str(Type *t) {
    if (t->type == VARIABLE) {
        char *ret = char_alloc(strlen(t->type_name));
        strcpy(ret, t->type_name);
        return ret;
    }
    else if(t->type == FUNCTION) {
        static const char arrow[] = "->";
        char *from = type2str(t->from);
        char *to = type2str(t->to);
        char *ret = char_alloc(strlen(from) + strlen(to) + 2);
        strscpy(ret, 3, from, arrow, to);
        free(from);
        free(to);
        return ret;
    } else {
        static const char par_left[] = "(";
        static const char par_right[] = ")";
        char *tmp = type2str(t->type_fn);
        char *ret = char_alloc(strlen(tmp) + 2);
        strscpy(ret, 3, par_left, tmp, par_right);
        free(tmp);
        return ret;
    }
}

int _typecmp(Type *a, Type *b) {
    if (a->type == VARIABLE && b->type == VARIABLE
            && strcmp(a->type_name, b->type_name) == 0) {
        return 1;
    }
    if (a->type == FUNCTION && b->type == FUNCTION) {
        int from_flag = _typecmp(a->from, b->from);
        int to_flag = _typecmp(a->to, b->to);
        return from_flag & to_flag;
    }
    else if (a->type == TYPE && b->type == TYPE) {
        return _typecmp(a->type_fn, b->type_fn);
    }
    else {
        return 0;
    }
}

// TODO : make more logical way
int typecmp(Type *a, Type *b) {
    char *s = type2str(a);
    char *t = type2str(b);
    return strcmp(s, t) == 0;
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
