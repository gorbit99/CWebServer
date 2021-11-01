#pragma once

#define eprintf(format, ...) \
    _eprintf_base(__FILE__, __LINE__, format, __VA_ARGS__)

#define eassert(condition, format, ...) \
    if (!(condition)) { \
        eprintf(format, __VA_ARGS__); \
    }

int _eprintf_base(char *file_name, int line_number, char *format, ...)
        __attribute__((format(printf, 3, 4)));
