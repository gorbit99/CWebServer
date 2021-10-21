#include "Errors.h"

#include <stdarg.h>
#include <stdio.h>

int _eprintf_base(char *file_name, int line_number, char *format, ...) {
    fprintf(stderr, "Error at %s:%d!\n", file_name, line_number);

    va_list args;
    va_start(args, format);
    int result = vfprintf(stderr, format, args);
    va_end(args);

    return result;
}
