#include "../tests/framework.h"

#include <assert.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>

#define GLUE(a, b)        a##b
#define EVAL(a, b)        GLUE(a, b)
#define GLUE4(a, b, c, d) a##b##c##d
#define EVAL4(a, b, c, d) GLUE4(a, b, c, d)
#define FUNC(action)      EVAL4(vector_, NAME, _, action)
#define VECTOR            EVAL(Vector, STRUCTNAME)

#ifdef TYPE

#ifdef DECLARE_VECTOR

typedef struct VECTOR VECTOR;

VECTOR *FUNC(new)(void);

void FUNC(free)(VECTOR *vector);

void FUNC(push_back)(VECTOR *vector, TYPE *elem);

void FUNC(insert)(VECTOR *vector, TYPE *elem, size_t index);

void FUNC(pop_back)(VECTOR *vector);

void FUNC(remove_at)(VECTOR *vector, size_t index);

TYPE *FUNC(get)(VECTOR *vector, size_t index);

size_t FUNC(size)(VECTOR *vector);

void FUNC(reserve)(VECTOR *vector, size_t capacity);

void FUNC(resize)(VECTOR *vector, size_t size, TYPE *default_value);

void FUNC(foreach)(VECTOR *vector, void (*func)(TYPE *t));

void FUNC(foreach_with_data)(VECTOR *vector,
                             void (*func)(TYPE *t, void *userdata),
                             void *userdata);

#undef DECLARE_VECTOR
#endif // DECLARE_VECTOR

#ifdef IMPLEMENT_VECTOR

#ifdef VECTOR_START_CAPACITY
#undef VECTOR_START_CAPACITY
#endif
#define VECTOR_START_CAPACITY 5

typedef struct VECTOR {
    TYPE *data;
    size_t size;
    size_t capacity;
} VECTOR;

VECTOR *FUNC(new)(void) {
    VECTOR *result = (VECTOR *)malloc(sizeof(VECTOR));
    result->data = (TYPE *)malloc(sizeof(TYPE) * VECTOR_START_CAPACITY),
    result->capacity = VECTOR_START_CAPACITY, result->size = 0;

    return result;
}

void FUNC(free)(VECTOR *vector) {
    free(vector->data);
}

void FUNC(push_back)(VECTOR *vector, TYPE *elem) {
    if (vector->capacity == vector->size) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(TYPE) * vector->capacity);
    }

    vector->data[vector->size] = *elem;
    vector->size++;
}

void FUNC(insert)(VECTOR *vector, TYPE *elem, size_t index) {
    if (vector->capacity == vector->size) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(TYPE) * vector->capacity);
    }

    memmove((char *)vector->data + (index + 1) * sizeof(TYPE),
            (char *)vector->data + index * sizeof(TYPE),
            (vector->size - index) * sizeof(TYPE));

    vector->data[index] = *elem;

    vector->size++;
}

void FUNC(pop_back)(VECTOR *vector) {
    vector->size--;
}

void FUNC(remove_at)(VECTOR *vector, size_t index) {
    memmove((char *)vector->data + index * sizeof(TYPE),
            (char *)vector->data + (index + 1) * sizeof(TYPE),
            (vector->size - index - 1) * sizeof(TYPE));

    vector->size--;
}

TYPE *FUNC(get)(VECTOR *vector, size_t index) {
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

    vector->data = realloc(vector->data, capacity * sizeof(TYPE));

    vector->capacity = capacity;
}

void FUNC(resize)(VECTOR *vector, size_t size, TYPE *default_value) {
    FUNC(reserve)(vector, size);

    if (vector->size < size) {
        for (size_t i = 0; i < size - vector->size; ++i) {
            vector->data[vector->size + i] = *default_value;
        }
    }

    vector->size = size;
}

void FUNC(foreach)(VECTOR *vector, void (*func)(TYPE *t)) {
    for (size_t i = 0; i < vector->size; ++i) {
        func(&vector->data[i]);
    }
}

void FUNC(foreach_with_data)(VECTOR *vector,
                             void (*func)(TYPE *t, void *userdata),
                             void *userdata) {
    for (size_t i = 0; i < vector->size; ++i) {
        func(&vector->data[i], userdata);
    }
}

#undef IMPLEMENT_VECTOR
#endif // IMPLEMENT_VECTOR

#undef TYPE
#undef NAME
#undef STRUCTNAME
#endif // TYPE

#ifndef VECTOR_TEST
#define VECTOR_TEST
TEST(vector);
#endif
