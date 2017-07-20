
void panic(char *s);
void _print_spaces(int space);

// str utils
void strscpy(char *buf, int count, ...);
char * char_alloc(int x);
char *create_substr(char *s, int st, int ed);
int find_first_split_point(char *s, char split_c);
