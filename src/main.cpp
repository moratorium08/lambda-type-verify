#include <cstdio>
#include <cstring>
#include <string.h>
#include "verify.h"

enum  {
    VERIFY_TYPES,
    INFER_TYPES
};

void help(char *cmd) {
    printf("[Usage]%s [option]\n", cmd);
    printf("- option -\n");
    printf(" -i : infer the types of your code\n");
    printf(" -v : verify the types of your code\n");
    printf(" -h : print this help\n");
}

int main(int argc, char *argv[]) {
    int verify_or_infer = INFER_TYPES;
    if (argc > 1) {
        char *sw = argv[1];
        if (strcmp(sw, "-i") == 0) {
            verify_or_infer = INFER_TYPES;
        }
        else if(strcmp(sw, "-v") == 0) {
            verify_or_infer = VERIFY_TYPES;
        }
        else {
            help(argv[0]);
            return 0;
        }
    }
    while (true) {
        char s[101];
        printf("> ");
        fgets(s, 100, stdin);
        if (strcmp(s, "q\n") == 0) {
            printf("Good Bye\n");
            break;
        }
        switch (verify_or_infer) {
            case VERIFY_TYPES:
            {
                int flag = verify(s);
                if (flag == 1) {
                    printf("OK\n");
                }
                else {
                    printf("NG\n");
                }
                break;
            }
            case INFER_TYPES:
            {
                Type* type = infer(s);
                print_type(type);
                break;
            }
        }
    }
    return 0;
}
