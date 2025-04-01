// Credit to
// https://stackoverflow.com/questions/40159892/using-asprintf-on-windows/49873938#49873938
// with modifications by me

#ifndef FMTSTR_H
#define FMTSTR_H

// MSVC does not implement vasprintf
#ifdef _MSC_VER
int vasprintf(char **strp, const char *format, va_list ap);
#endif

char* str_fmt(const char *format, ...);

#endif
