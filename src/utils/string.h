#pragma once

#include "Vector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct String String;

#define TYPE       String *
#define NAME       string
#define STRUCTNAME String
#define DECLARE_VECTOR
#include "Vector.h"

String *string_new(void);

String *string_from_cstr(const char *str);

String *string_from_cstr_part(const char *str, size_t len);

// TODO TEST
char string_get(String *string, size_t index);

String *string_read_upto(FILE *file, const char *delims);

String *string_read_line(FILE *file);

String *string_read_word(FILE *file);

String *string_read_len(FILE *file, size_t len);

void string_free(String *string);

void string_push_back(String *string, char c);

void string_pop_back(String *string);

// TODO Test
void string_push_front(String *string, char c);

// TODO Test
void string_pop_front(String *string);

void string_concat(String *to, String *from);

int string_scanf(String *string, const char *format, ...)
        __attribute__((format(scanf, 2, 3)));

int string_strcmp(String *string, String *cmp);

int string_cstrcmp(String *string, const char *cmp);

// TODO TEST
int string_strcmp_case_insensitive(String *string, String *cmp);

// TODO TEST
int string_cstrcmp_case_insensitive(String *string, const char *cmp);

VectorString *string_split(String *string,
                           const char *separator,
                           bool keep_empty,
                           size_t max_parts);

char *string_as_cstr(String *string);

void string_map(String *string, char (*func)(char c));

void string_trim(String *string);

// TODO Test
size_t string_size(String *string);

char string_map_tolower(char c);

char string_map_toupper(char c);

uint64_t string_hash(void *data);

int string_cmp(void *key1, void *key2);

TEST(string);
