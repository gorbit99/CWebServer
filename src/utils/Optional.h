#pragma once

#include "../tests/framework.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define GLUE(a, b)        a##b
#define EVAL(a, b)        GLUE(a, b)
#define GLUE4(a, b, c, d) a##b##c##d
#define EVAL4(a, b, c, d) GLUE4(a, b, c, d)
#define FUNC(action)      EVAL4(optional_, NAME, _, action)
#define OPTIONAL          EVAL(Optional, STRUCTNAME)

#ifdef TYPE

typedef struct OPTIONAL OPTIONAL;

#ifdef DECLARE_OPTIONAL
OPTIONAL *FUNC(new)(void);

void FUNC(free)(OPTIONAL *optional);

void FUNC(set)(OPTIONAL *optional, TYPE *value);

void FUNC(reset)(OPTIONAL *optional);

bool FUNC(has_value)(OPTIONAL *optional);

TYPE *FUNC(value_or)(OPTIONAL *optional, TYPE *placeholder);

void FUNC(map)(OPTIONAL *optional, void (*func)(TYPE *val));

#undef DECLARE_OPTIOPTIONAL
#endif // DECLARE_OPTIONAL

#ifdef IMPLEMENT_OPTIONAL

struct OPTIONAL {
    TYPE *data;
    bool has_value;
};

OPTIONAL *FUNC(new)(void) {
    OPTIONAL *result = (OPTIONAL *)malloc(sizeof(OPTIONAL));
    result->data = (TYPE *)malloc(sizeof(TYPE));
    result->has_value = false;

    return result;
}

void FUNC(free)(OPTIONAL *optional) {
    free(optional->data);
    free(optional);
}

void FUNC(set)(OPTIONAL *optional, TYPE *value) {
    *optional->data = *value;
    optional->has_value = true;
}

void FUNC(reset)(OPTIONAL *optional) {
    optional->has_value = false;
}

bool FUNC(has_value)(OPTIONAL *optional) {
    return optional->has_value;
}

TYPE *FUNC(value_or)(OPTIONAL *optional, TYPE *placeholder) {
    return optional->has_value ? optional->data : placeholder;
}

void FUNC(map)(OPTIONAL *optional, void (*func)(TYPE *val)) {
    if (optional->has_value) {
        func(optional->data);
    }
}

#undef IMPLEMENT_OPTIONAL
#endif // IMPLEMENT_OPTIONAL

#undef TYPE
#undef NAME
#undef STRUCTNAME

#undef GLUE
#undef EVAL
#undef GLUE4
#undef EVAL4
#undef FUNC
#undef OPTIONAL
#endif // TYPE

TEST(optional);
