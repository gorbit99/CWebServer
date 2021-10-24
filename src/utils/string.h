#pragma once

#include "Vector.h"

#include <stddef.h>
#include <stdio.h>

typedef struct String String;

String *string_new();

String *string_from_cstr(char *str);

String *string_from_cstr_part(char *str, size_t len);

String *string_read_upto(FILE *file, char *delims);

String *string_read_line(FILE *file);

String *string_read_word(FILE *file);

String *string_read_len(FILE *file, size_t len);

void string_free(String *string);

void string_add(String *string, char c);

void string_pop_back(String *string);

void string_concat(String *to, String *from);

int string_scanf(String *string, char *format, ...);

int string_strcmp(String *string, String *cmp);

int string_cstrcmp(String *string, char *cmp);

Vector *string_split(String *string, char *separator, bool keep_empty,
                     size_t max_parts);

char *string_as_cstr(String *string);

void string_map(String *string, char (*func)(char c));

TEST(string);
