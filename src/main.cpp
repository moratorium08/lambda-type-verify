#include <stdio.h>
#include "verify.h"

int main(void) {
    char s[101];
    fgets(s, 100, stdin);
    int flag = verify(s);
    if (flag == 1) {
        printf("OK\n");
    } else {
        printf("NG\n");
    }
    return 0;
}
