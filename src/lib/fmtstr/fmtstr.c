// Credit to
// https://stackoverflow.com/a/49873938
// with modifications by me

#include <stdarg.h> /* needed for va_*         */
#include <stdio.h>  /* needed for vsnprintf    */
#include <stdlib.h> /* needed for malloc, free */

#ifdef _MSC_VER
#include <direct.h>
#else
#include <linux/limits.h>
#endif

#ifdef _MSC_VER
/*
 * vasprintf:
 * MSVC does not implement these, thus we implement them here
 * GNU-C-compatible compilers implement these with the same names, and we
 * don't have to do anything
 */
int vasprintf(char **strp, const char *format, va_list ap) {
    int len = _vscprintf(format, ap);
    if (len == -1)
        return -1;
    char *str = (char *)malloc((size_t)len + 1);
    if (!str)
        return -1;
    int retval = vsnprintf(str, len + 1, format, ap);
    if (retval == -1) {
        free(str);
        return -1;
    }
    *strp = str;
    return retval;
}
#endif

char *str_fmt(const char *format, ...) {
    char *str = NULL;
    va_list ap;
    va_start(ap, format);
    vasprintf(&str, format, ap);
    va_end(ap);
    return str;
}
