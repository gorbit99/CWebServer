#include "../tests/framework.h"

#include <assert.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>

#if !defined(VECTORTYPE) || !defined(VECTORFUNCNAME) \
        || !defined(VECTORSTRUCTNAME)
#error Invalid Vector include!
#endif

#define GLUE(a, b)        a##b
#define EVAL(a, b)        GLUE(a, b)
#define GLUE4(a, b, c, d) a##b##c##d
#define EVAL4(a, b, c, d) GLUE4(a, b, c, d)
#define FUNC(action)      EVAL4(vector_, VECTORFUNCNAME, _, action)
#define VECTOR            EVAL(Vector, VECTORSTRUCTNAME)

#ifdef DECLARE_VECTOR
typedef struct VECTOR VECTOR;

VECTOR *FUNC(new)(void);

void FUNC(free)(VECTOR *vector);

void FUNC(push_back)(VECTOR *vector, VECTORTYPE *elem);

void FUNC(insert)(VECTOR *vector, VECTORTYPE *elem, size_t index);

void FUNC(pop_back)(VECTOR *vector);

void FUNC(remove_at)(VECTOR *vector, size_t index);

VECTORTYPE *FUNC(get)(VECTOR *vector, size_t index);

size_t FUNC(size)(VECTOR *vector);

void FUNC(reserve)(VECTOR *vector, size_t capacity);

void FUNC(resize)(VECTOR *vector, size_t size, VECTORTYPE *default_value);

void FUNC(foreach)(VECTOR *vector, void (*func)(VECTORTYPE *t));

void FUNC(foreach_with_data)(VECTOR *vector,
                             void (*func)(VECTORTYPE *t, void *userdata),
                             void *userdata);

#undef DECLARE_VECTOR
#endif

#ifdef IMPLEMENT_VECTOR

#ifdef VECTOR_START_CAPACITY
#undef VECTOR_START_CAPACITY
#endif
#define VECTOR_START_CAPACITY 5

typedef struct VECTOR {
    VECTORTYPE *data;
    size_t size;
    size_t capacity;
} VECTOR;

VECTOR *FUNC(new)(void) {
    VECTOR *result = (VECTOR *)malloc(sizeof(VECTOR));
    result->data =
            (VECTORTYPE *)malloc(sizeof(VECTORTYPE) * VECTOR_START_CAPACITY),
    result->capacity = VECTOR_START_CAPACITY, result->size = 0;

    return result;
}

void FUNC(free)(VECTOR *vector) {
    free(vector->data);
    free(vector);
}

void FUNC(push_back)(VECTOR *vector, VECTORTYPE *elem) {
    if (vector->capacity == vector->size) {
        vector->capacity *= 2;
        vector->data =
                realloc(vector->data, sizeof(VECTORTYPE) * vector->capacity);
    }

    vector->data[vector->size] = *elem;
    vector->size++;
}

void FUNC(insert)(VECTOR *vector, VECTORTYPE *elem, size_t index) {
    if (vector->capacity == vector->size) {
        vector->capacity *= 2;
        vector->data =
                realloc(vector->data, sizeof(VECTORTYPE) * vector->capacity);
    }

    memmove((char *)vector->data + (index + 1) * sizeof(VECTORTYPE),
            (char *)vector->data + index * sizeof(VECTORTYPE),
            (vector->size - index) * sizeof(VECTORTYPE));

    vector->data[index] = *elem;

    vector->size++;
}

void FUNC(pop_back)(VECTOR *vector) {
    vector->size--;
}

void FUNC(remove_at)(VECTOR *vector, size_t index) {
    memmove((char *)vector->data + index * sizeof(VECTORTYPE),
            (char *)vector->data + (index + 1) * sizeof(VECTORTYPE),
            (vector->size - index - 1) * sizeof(VECTORTYPE));

    vector->size--;
}

VECTORTYPE *FUNC(get)(VECTOR *vector, size_t index) {
    assert(vector->size > index);

    return &vector->data[index];
}

size_t FUNC(size)(VECTOR *vector) {
    return vector->size;
}

void FUNC(reserve)(VECTOR *vector, size_t capacity) {
    if (vector->capacity > capacity) {
        return;
    }

    vector->data = realloc(vector->data, capacity * sizeof(VECTORTYPE));

    vector->capacity = capacity;
}

void FUNC(resize)(VECTOR *vector, size_t size, VECTORTYPE *default_value) {
    FUNC(reserve)(vector, size);

    if (vector->size < size) {
        for (size_t i = 0; i < size - vector->size; ++i) {
            vector->data[vector->size + i] = *default_value;
        }
    }

    vector->size = size;
}

void FUNC(foreach)(VECTOR *vector, void (*func)(VECTORTYPE *t)) {
    for (size_t i = 0; i < vector->size; ++i) {
        func(&vector->data[i]);
    }
}

void FUNC(foreach_with_data)(VECTOR *vector,
                             void (*func)(VECTORTYPE *t, void *userdata),
                             void *userdata) {
    for (size_t i = 0; i < vector->size; ++i) {
        func(&vector->data[i], userdata);
    }
}

#undef IMPLEMENT_VECTOR
#endif // IMPLEMENT_VECTOR

#undef VECTORTYPE
#undef VECTORFUNCNAME
#undef VECTORSTRUCTNAME

#undef GLUE
#undef EVAL
#undef GLUE4
#undef EVAL4
#undef FUNC
#undef VECTOR

#ifndef VECTOR_TEST
#define VECTOR_TEST
TEST(vector);
#endif
