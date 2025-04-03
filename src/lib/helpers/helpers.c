#ifndef LIBHELPERS_C
#define LIBHELPERS_C

#include <ctype.h>
#include <string.h>

char *str_lower(char *in_str) {
    char *str = strdup(in_str);
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

#endif
