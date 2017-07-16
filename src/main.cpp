#include <stdio.h>
#include "verify.h"

int main(void) {
    while (true) {
        char s[101];
        fgets(s, 100, stdin);
        if (strcmp(s, "q\n") == 0) {
            printf("Good Bye\n");
            break;
        }
        int flag = verify(s);
        if (flag == 1) {
            printf("OK\n");
        } else {
            printf("NG\n");
        }
    }
    return 0;
}
