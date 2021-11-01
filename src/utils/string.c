#include "string.h"

#include "Vector.h"
#include "cleanup.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define TYPE       String *
#define NAME       string
#define STRUCTNAME String
#define IMPLEMENT_VECTOR
#include "Vector.h"

static const size_t STRING_START_CAPACITY = 5;

struct String {
    char *data;
    size_t size;
    size_t capacity;
};

String *string_new(void) {
    String *result = (String *)malloc(sizeof(String));

    result->data = (char *)malloc(sizeof(char) * (STRING_START_CAPACITY + 1));
    result->data[0] = '\0';
    result->size = 0;
    result->capacity = STRING_START_CAPACITY;

    return result;
}

String *string_from_cstr(const char *str) {
    String *result = (String *)malloc(sizeof(String));

    size_t str_len = strlen(str);

    result->data = (char *)malloc(sizeof(char) * (str_len + 1));
    result->size = str_len;
    result->capacity = str_len;

    strcpy(result->data, str);

    return result;
}

String *string_from_cstr_part(const char *str, size_t len) {
    String *result = (String *)malloc(sizeof(String));

    size_t str_len = strlen(str);

    if (str_len > len) {
        str_len = len;
    }

    result->data = (char *)malloc(sizeof(char) * (str_len + 1));
    result->size = str_len;
    result->capacity = str_len;
    result->data[str_len] = '\0';

    strncpy(result->data, str, str_len);

    return result;
}

char string_get(String *string, size_t index) {
    assert(index < string->size);

    return string->data[index];
}

String *string_read_upto(FILE *file, const char *delims) {
    String *result = string_new();
    char c;
    while (fscanf(file, "%c", &c) != EOF) {
        if (strchr(delims, c) != NULL) {
            break;
        }

        string_push_back(result, c);
    }

    return result;
}

String *string_read_line(FILE *file) {
    return string_read_upto(file, "\n");
}

String *string_read_word(FILE *file) {
    return string_read_upto(file, " \t\n\r\v\f");
}

String *string_read_len(FILE *file, size_t len) {
    String *result = (String *)malloc(sizeof(String));

    result->data = (char *)malloc(sizeof(char) * (len + 1));
    memset(result->data, 0, sizeof(char) * (len + 1));
    size_t read = fread(result->data, sizeof(char), len, file);
    result->size = read;
    result->data = (char *)realloc(result->data, sizeof(char) * (read + 1));
    result->capacity = read;

    return result;
}

void string_free(String *string) {
    free(string->data);
    free(string);
}

void string_push_back(String *string, char c) {
    if (string->capacity == string->size) {
        string->capacity *= 2;
        string->data = (char *)realloc(string->data,
                                       sizeof(char) * (string->capacity + 1));
    }

    string->data[string->size] = c;
    string->data[string->size + 1] = '\0';
    string->size++;
}

void string_pop_back(String *string) {
    if (string->size == 0) {
        return;
    }
    string->size--;
    string->data[string->size] = '\0';
}

void string_push_front(String *string, char c) {
    if (string->capacity == string->size) {
        string->capacity *= 2;
        string->data = (char *)realloc(string->data,
                                       sizeof(char) * (string->capacity + 1));
    }

    memmove(string->data + 1, string->data, (string->size + 1) * sizeof(char));
    string->data[0] = c;
    string->size++;
}

void string_pop_front(String *string) {
    if (string->size == 0) {
        return;
    }
    string->size--;
    memmove(string->data, string->data + 1, (string->size + 1) * sizeof(char));
    string->data[string->size] = '\0';
}

void string_concat(String *to, String *from) {
    to->capacity = to->capacity + from->capacity;
    to->data = (char *)realloc(to->data, sizeof(char) * (to->capacity + 1));
    to->size = to->size + from->size;
    strcat(to->data, from->data);
}

int string_scanf(String *string, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int result = vsscanf(string->data, format, args);

    va_end(args);

    return result;
}

int string_strcmp(String *string, String *cmp) {
    return strcmp(string->data, cmp->data);
}

int string_cstrcmp(String *string, const char *cmp) {
    return strcmp(string->data, cmp);
}

int string_strcmp_case_insensitive(String *string, String *cmp) {
    return strcasecmp(string->data, cmp->data);
}

int string_cstrcmp_case_insensitive(String *string, const char *cmp) {
    return strcasecmp(string->data, cmp);
}

VectorString *string_split(String *string,
                           const char *separator,
                           bool keep_empty,
                           size_t max_parts) {
    VectorString *result = vector_string_new();

    size_t sep_len = strlen(separator);

    char *prev_front = string->data;

    size_t part_count = 0;

    char *next_separator = prev_front;
    while (next_separator != NULL
           && (part_count != max_parts || max_parts == 0)) {
        next_separator = strstr(prev_front, separator);

        size_t len;
        if (next_separator == NULL
            || (max_parts != 0 && part_count == max_parts - 1)) {
            len = strlen(prev_front);
        } else {
            len = (size_t)(next_separator - prev_front);
        }

        if (len == 0 && !keep_empty) {
            prev_front += sep_len;
            continue;
        }

        String *part = string_from_cstr_part(prev_front, len);

        vector_string_push_back(result, &part);
        part_count++;

        prev_front += len + sep_len;
    }

    return result;
}

char *string_as_cstr(String *string) {
    return string->data;
}

void string_map(String *string, char (*func)(char c)) {
    char *str = string->data;

    while (*str != '\0') {
        *str = func(*str);

        str++;
    }
}

void string_trim(String *string) {
    if (string->size == 0) {
        return;
    }

    size_t start = 0;
    while (isspace(string->data[start])) {
        start++;
    }

    if (string->data[start] == '\0') {
        string->data[0] = '\0';
        string->size = 0;
        return;
    }

    size_t end = string->size - 1;
    while (isspace(string->data[end])) {
        end--;
    }
    end++;

    memmove(string->data, string->data + start, (end - start) * sizeof(char));
    string->data[end - start] = '\0';
    string->size = end - start;
}

size_t string_size(String *string) {
    return string->size;
}

char string_map_tolower(char c) {
    return (char)tolower(c);
}

char string_map_toupper(char c) {
    return (char)toupper(c);
}

uint64_t string_hash(void *data) {
    String *string = *(String **)data;
    uint64_t hash = 0;

    uint64_t current = 0;
    for (size_t i = 0; i < string->size; i++) {
        uint64_t converted = (uint64_t)string->data[i] & 0xff;

        current = current << 8 | converted;

        if (i % 4 == 3) {
            hash ^= current;
            current = 0;
        }
    }
    return hash;
}

int string_cmp(void *key1, void *key2) {
    String *a = *(String **)key1;
    String *b = *(String **)key2;

    return string_strcmp(a, b);
}

TEST(string_new) {
    STARTTEST();

    String *string = string_new();

    TESTASSERT(String starts with size 0, string->size == 0);
    TESTASSERT(String starts with capacity 5, string->capacity == 5);
    TESTASSERT(String is empty, strcmp(string->data, "") == 0);

    string_free(string);

    ENDTEST();
}

TEST(string_from_cstr) {
    STARTTEST();

    String *string = string_from_cstr("Hello World");

    TESTASSERT(String has a length of 11, string->size == 11);
    TESTASSERT(String has 'Hello World' in it,
               strcmp(string->data, "Hello World") == 0);

    string_free(string);

    ENDTEST();
}

TEST(string_from_cstr_part) {
    STARTTEST();

    String *string = string_from_cstr_part("Hello World", 5);

    TESTASSERT(String has a length of 5, string->size == 5);
    TESTASSERT(String has 'Hello' in it, strcmp(string->data, "Hello") == 0);

    string_free(string);

    ENDTEST();
}

TEST(string_read_upto) {
    STARTTEST();

    char buffer[] = "This is a test line\nThis is test line two\n";
    size_t size = strlen(buffer);
    FILE *test_file = fmemopen(buffer, size, "r");

    String *string = string_read_upto(test_file, "t");

    TESTASSERT(String has a length of 10, string->size == 10);
    TESTASSERT(String has 'This is a ' in it,
               strcmp(string->data, "This is a ") == 0);

    string_free(string);
    fclose(test_file);

    ENDTEST();
}

TEST(string_read_line) {
    STARTTEST();

    char buffer[] = "This is a test line\nThis is test line two\n";
    size_t size = strlen(buffer);
    FILE *test_file = fmemopen(buffer, size, "r");

    String *string = string_read_line(test_file);

    TESTASSERT(String has a length of 19, string->size == 19);
    TESTASSERT(String has 'This is a test line' in it,
               strcmp(string->data, "This is a test line") == 0);

    string_free(string);
    fclose(test_file);

    ENDTEST();
}

TEST(string_read_word) {
    STARTTEST();

    char buffer[] = "This is a test line\nThis is test line two\n";
    size_t size = strlen(buffer);
    FILE *test_file = fmemopen(buffer, size, "r");

    String *string = string_read_word(test_file);

    TESTASSERT(String has a length of 4, string->size == 4);
    TESTASSERT(String has 'This' in it, strcmp(string->data, "This") == 0);

    string_free(string);
    fclose(test_file);

    ENDTEST();
}

TEST(string_add) {
    STARTTEST();

    String *string = string_new();

    string_push_back(string, 'a');
    string_push_back(string, 'b');
    string_push_back(string, 'c');
    string_push_back(string, 'd');

    TESTASSERT(String has a length of 4, string->size == 4);
    TESTASSERT(String has 'abcd' in it, strcmp(string->data, "abcd") == 0);

    string_free(string);

    ENDTEST();
}

TEST(string_concat) {
    STARTTEST();

    String *string1 = string_from_cstr("Hello ");
    String *string2 = string_from_cstr("World");

    string_concat(string1, string2);

    TESTASSERT(String1 has a size of 11, string1->size == 11);
    TESTASSERT(String2 has a size of 5, string2->size == 5);
    TESTASSERT(String1 has 'Hello World' in it,
               strcmp(string1->data, "Hello World") == 0);
    TESTASSERT(String2 has 'World' in it, strcmp(string2->data, "World") == 0);

    string_free(string1);
    string_free(string2);

    ENDTEST();
}

TEST(string_scanf) {
    STARTTEST();

    String *string = string_from_cstr("15");

    int i;
    string_scanf(string, "%d", &i);

    TESTASSERT(The number is 15, i == 15);

    string_free(string);

    ENDTEST();
}

TEST(string_strcmp) {
    STARTTEST();

    String *string = string_from_cstr("Hello World");
    String *string_same = string_from_cstr("Hello World");
    String *string_less = string_from_cstr("Helko World");
    String *string_more = string_from_cstr("Hello Xorld");

    TESTASSERT(Same string results in 0,
               string_strcmp(string, string_same) == 0);
    TESTASSERT(Alphabetically smaller string results in 1,
               string_strcmp(string, string_less) == 1);
    TESTASSERT(Alphabetically bigger string results in - 1,
               string_strcmp(string, string_more) == -1);

    string_free(string);
    string_free(string_same);
    string_free(string_less);
    string_free(string_more);

    ENDTEST();
}

TEST(string_cstrcmp) {
    STARTTEST();

    String *string = string_from_cstr("Hello World");

    TESTASSERT(Same string results in 0,
               string_cstrcmp(string, "Hello World") == 0);
    TESTASSERT(Alphabetically smaller string results in 1,
               string_cstrcmp(string, "Helko World") == 1);
    TESTASSERT(Alphabetically bigger string results in - 1,
               string_cstrcmp(string, "Hello Xorld") == -1);

    string_free(string);

    ENDTEST();
}

TEST(string_split) {
    STARTTEST();

    String *original = string_from_cstr("Comma, separated, list");
    char *parts[] = {"Comma", "separated", "list"};

    VectorString *split = string_split(original, ", ", true, 0);

    TESTASSERT(The result has 3 parts, vector_string_size(split) == 3);

    for (size_t i = 0; i < 3; i++) {
        String *str = *vector_string_get(split, i);

        TESTASSERT(The parts match, strcmp(str->data, parts[i]));

        string_free(str);
    }

    string_free(original);
    vector_string_free(split);

    ENDTEST();
}

TEST(string) {
    STARTTEST();

    RUNTEST(string_new);
    RUNTEST(string_from_cstr);
    RUNTEST(string_from_cstr_part);
    RUNTEST(string_read_upto);
    RUNTEST(string_read_line);
    RUNTEST(string_read_word);
    RUNTEST(string_add);
    RUNTEST(string_concat);
    RUNTEST(string_scanf);
    RUNTEST(string_strcmp);
    RUNTEST(string_cstrcmp);

    PRINTTESTSUITE(String);

    ENDTEST();
}
