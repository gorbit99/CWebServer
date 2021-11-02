#include "../tests/framework.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#if !defined(OPTIONALTYPE) || !defined(OPTIONALFUNCNAME) \
        || !defined(OPTIONALSTRUCTNAME)
#error Invalid optional include!
#endif

#define GLUE(a, b)        a##b
#define EVAL(a, b)        GLUE(a, b)
#define GLUE4(a, b, c, d) a##b##c##d
#define EVAL4(a, b, c, d) GLUE4(a, b, c, d)
#define FUNC(action)      EVAL4(optional_, OPTIONALFUNCNAME, _, action)
#define OPTIONAL          EVAL(Optional, OPTIONALSTRUCTNAME)

#ifdef DECLARE_OPTIONAL
typedef struct OPTIONAL OPTIONAL;

OPTIONAL *FUNC(new)(void);

void FUNC(free)(OPTIONAL *optional);

void FUNC(set)(OPTIONAL *optional, OPTIONALTYPE *value);

void FUNC(reset)(OPTIONAL *optional);

bool FUNC(has_value)(OPTIONAL *optional);

OPTIONALTYPE *FUNC(value_or)(OPTIONAL *optional, OPTIONALTYPE *placeholder);

void FUNC(map)(OPTIONAL *optional, void (*func)(OPTIONALTYPE *val));

#undef DECLARE_OPTIONAL
#endif // DECLARE_OPTIONAL

#ifdef IMPLEMENT_OPTIONAL

typedef struct OPTIONAL {
    OPTIONALTYPE *data;
    bool has_value;
} OPTIONAL;

OPTIONAL *FUNC(new)(void) {
    OPTIONAL *result = (OPTIONAL *)malloc(sizeof(OPTIONAL));
    result->data = (OPTIONALTYPE *)malloc(sizeof(OPTIONALTYPE));
    result->has_value = false;

    return result;
}

void FUNC(free)(OPTIONAL *optional) {
    free(optional->data);
    free(optional);
}

void FUNC(set)(OPTIONAL *optional, OPTIONALTYPE *value) {
    *optional->data = *value;
    optional->has_value = true;
}

void FUNC(reset)(OPTIONAL *optional) {
    optional->has_value = false;
}

bool FUNC(has_value)(OPTIONAL *optional) {
    return optional->has_value;
}

OPTIONALTYPE *FUNC(value_or)(OPTIONAL *optional, OPTIONALTYPE *placeholder) {
    return optional->has_value ? optional->data : placeholder;
}

void FUNC(map)(OPTIONAL *optional, void (*func)(OPTIONALTYPE *val)) {
    if (optional->has_value) {
        func(optional->data);
    }
}

#undef IMPLEMENT_OPTIONAL
#endif // IMPLEMENT_OPTIONAL

#undef OPTIONALTYPE
#undef OPTIONALFUNCNAME
#undef OPTIONALSTRUCTNAME

#undef GLUE
#undef EVAL
#undef GLUE4
#undef EVAL4
#undef FUNC
#undef OPTIONAL

#ifndef OPTIONAL_TESTS
#define OPTIONAL_TESTS
TEST(optional);
#endif // OPTIONAL_TESTS
