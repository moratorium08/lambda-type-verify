#ifndef TYPE_H
#define TYPE_H
#include <vector>
using namespace std;
enum {
    FUNCTION,
    VARIABLE,
    TYPE,
    UNKNOWN
};

typedef struct Type {
    int type;
    union {
        char* type_name;
        Type* type_fn;
        int id;
        struct {
            Type *from;
            Type *to;
        };
    };
} Type;


void print_type(Type *type);
void _print_type(Type *type);
Type *make_type();
Type * apply_type(Type *left_t, Type *right_t);
int typecmp(Type *a, Type *b);
Type * make_func_type(Type *from, Type *to);
Type * make_primitive(char * type_name);
Type * make_unknown_type(int id);
Type * make_type_from_str(char *s, int type_wrap_if_func);
void typecpy(Type *a, Type *b);
char *type2str(Type *t);
#endif /* TYPE_H */
