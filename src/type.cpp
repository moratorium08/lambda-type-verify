#include <cstdlib>
#include "ast.h"
#include "util.h"
#include "type.h"

// a <- b, free(b)
void typecpy(Type *a, Type *b) {
    if (a == b) return;
    a->type = b->type;
    switch (b->type) {
        case FUNCTION:
            a->from = b->from;
            a->to = b->to;
            break;
        case VARIABLE:
            a->type_name = b->type_name;
            break;
        case TYPE:
            a->type_fn = b->type_fn;
            break;
        case UNKNOWN:
            a->id = b->id;
            break;
    }
}

Type *make_unknown_type(int id){
    Type *type = (Type*)malloc(sizeof(Type));
    type->type = UNKNOWN;
    type->id = id;
    return type;
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
    }
    else if(t->type == UNKNOWN) {
        char *ret = char_alloc(21);
        sprintf(ret, "?%d", t->id);
        return ret;
    }
    else {
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
    }
    else if(t->type == UNKNOWN) {
        printf("?%d", t->id);
    }
    else {
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
