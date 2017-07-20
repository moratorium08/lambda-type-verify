#include <cstring>
#include <stdarg.h>
#include <cstdlib>
#include <cstdio>

void panic(char *s) {
    printf("%s", s);
    exit(-1);
}

void _print_spaces(int space) {
    for (int i = 0 ; i < space; i++) {
        printf(" ");
    }
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
    buf[idx] = 0;
}

char * char_alloc(int x) {
    char *ret = (char *)malloc(x + 1);
    memset(ret, x + 1, 0);
    return ret;
}

char *create_substr(char *s, int st, int ed) {
    char *ret = (char *)malloc(ed - st + 1);
    for (int i = st; i < ed; i++) {
        ret[i - st] = s[i];
    }
    ret[ed - st] = 0;
    return ret;
}

int find_first_split_point(char *s, char split_c) {
    int l = strlen(s);
    int i;
    for (i = 0; i < l; i++)
        if (split_c == s[i])
            break;
    return i;
}
