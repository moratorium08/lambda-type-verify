#include <stdio.h>
#include "verify.h"

int main(void) {
    char s[101];
    scanf("%100s", s);
    int flag = verify(s);
    if (flag == 1) {
        printf("OK\n");
    } else {
        printf("NG\n");
    }
    return 0;
}
