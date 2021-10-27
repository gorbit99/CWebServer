#include "Vector.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

const size_t VECTOR_START_CAPACITY = 5;

struct Vector {
    void *data;
    size_t data_size;
    size_t size;
    size_t capacity;
};

Vector *_vector_new_base(size_t data_size) {
    Vector *result = (Vector *)malloc(sizeof(Vector));

    result->data_size = data_size;
    result->capacity = VECTOR_START_CAPACITY;
    result->data = malloc(data_size * VECTOR_START_CAPACITY);
    result->size = 0;

    return result;
}

void vector_free(Vector *vector) {
    free(vector->data);
    free(vector);
}

void vector_push_back(Vector *vector, void *elem) {
    if (vector->capacity == vector->size) {
        vector->capacity *= 2;
        vector->data =
                realloc(vector->data, vector->data_size * vector->capacity);
    }

    memcpy((char *)vector->data + vector->size * vector->data_size, elem,
           vector->data_size);

    vector->size++;
}

void vector_insert(Vector *vector, void *elem, size_t index) {
    if (vector->capacity == vector->size) {
        vector->capacity *= 2;
        vector->data =
                realloc(vector->data, vector->data_size * vector->capacity);
    }

    memmove((char *)vector->data + (index + 1) * vector->data_size,
            (char *)vector->data + index * vector->data_size,
            (vector->size - index) * vector->data_size);

    memcpy((char *)vector->data + index * vector->data_size, elem,
           vector->data_size);

    vector->size++;
}

void vector_pop_back(Vector *vector) {
    vector->size--;
}

void vector_remove_at(Vector *vector, size_t index) {
    memmove((char *)vector->data + index * vector->data_size,
            (char *)vector->data + (index + 1) * vector->data_size,
            (vector->size - index - 1) * vector->data_size);

    vector->size--;
}

void *_vector_get_base(Vector *vector, size_t index) {
    assert(vector->size > index);

    return ((char *)vector->data) + index * vector->data_size;
}

size_t vector_size(Vector *vector) {
    return vector->size;
}

void vector_reserve(Vector *vector, size_t capacity) {
    if (vector->capacity > capacity) {
        return;
    }

    vector->data = realloc(vector->data, capacity * vector->data_size);

    vector->capacity = capacity;
}

void vector_resize(Vector *vector, size_t size, void *default_value) {
    vector_reserve(vector, size);

    if (vector->size < size) {
        for (size_t i = 0; i < size - vector->size; ++i) {
            memcpy((char *)vector->data
                           + (vector->size + i) * vector->data_size,
                   default_value, vector->data_size);
        }
    }

    vector->size = size;
}

void vector_foreach(Vector *vector, void (*func)(void *)) {
    char *steppable = (char *)vector->data;

    for (size_t i = 0; i < vector->size; i++) {
        func((void *)steppable);
        steppable += vector->data_size;
    }
}

TEST(vector_new) {
    STARTTEST();

    Vector *vector = vector_new(int);

    TESTASSERT(The default size of the vector is 0, vector->size == 0);
    TESTASSERT(The default capacity of the vector is the defined one,
               vector->capacity == VECTOR_START_CAPACITY);
    TESTASSERT(The data size of the vector is 4, vector->data_size == 4);

    vector_free(vector);

    ENDTEST();
}

TEST(vector_push_back_and_get) {
    STARTTEST();

    Vector *vector = vector_new(int);
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);

    TESTASSERT(The size of the vector is 7, vector->size == 7);
    TESTASSERT(The capacity of the vector is 10, vector->capacity == 10);

    TESTASSERT(The first element is 5,
               *(int *)_vector_get_base(vector, 0) == 5);
    TESTASSERT(The fourth element is 7,
               *(int *)_vector_get_base(vector, 3) == 7);

    vector_free(vector);

    ENDTEST();
}

TEST(vector_insert) {
    STARTTEST();

    Vector *vector = vector_new(int);
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    int elem4 = 10;
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);
    vector_insert(vector, &elem4, 2);

    TESTASSERT(The size of the vector is 4, vector->size == 4);
    TESTASSERT(The capacity of the vector is 5, vector->capacity == 5);

    TESTASSERT(The first element is 5,
               *(int *)_vector_get_base(vector, 0) == 5);
    TESTASSERT(The third element is 10,
               *(int *)_vector_get_base(vector, 2) == 10);

    vector_free(vector);

    ENDTEST();
}

TEST(vector_pop_back) {
    STARTTEST();

    Vector *vector = vector_new(int);

    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);

    vector_pop_back(vector);

    TESTASSERT(The size of the vector is 2, vector->size == 2);

    vector_free(vector);

    ENDTEST();
}

TEST(vector_remove_at) {
    STARTTEST();

    Vector *vector = vector_new(int);

    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem1);

    vector_remove_at(vector, 2);

    TESTASSERT(The size of the vector is 4, vector->size == 4);
    TESTASSERT(The second element is 7,
               *(int *)_vector_get_base(vector, 1) == 7);
    TESTASSERT(The third element is 7,
               *(int *)_vector_get_base(vector, 2) == 7);

    vector_free(vector);

    ENDTEST();
}

TEST(vector_reserve) {
    STARTTEST();

    Vector *vector = vector_new(int);
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);

    vector_reserve(vector, 2);
    TESTASSERT(If the capacity is smaller than the current one do nothing,
               vector->capacity == 5);

    vector_reserve(vector, 20);
    TESTASSERT(Otherwise increase capacity, vector->capacity == 20);
    TESTASSERT(Dont touch the size, vector->size == 3);

    vector_free(vector);

    ENDTEST();
}

TEST(vector_resize) {
    STARTTEST();

    Vector *vector = vector_new(int);
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_push_back(vector, &elem1);
    vector_push_back(vector, &elem2);
    vector_push_back(vector, &elem3);

    int filler = 54;
    vector_resize(vector, 10, &filler);

    TESTASSERT(The size should be 10, vector->size == 10);
    for (size_t i = 3; i < 10; ++i) {
        TESTASSERT(Every new element is 54,
                   *(int *)_vector_get_base(vector, i) == 54);
    }

    vector_free(vector);

    ENDTEST();
}

TEST(vector) {
    STARTTEST();

    RUNTEST(vector_new);
    RUNTEST(vector_push_back_and_get);
    RUNTEST(vector_insert);
    RUNTEST(vector_pop_back);
    RUNTEST(vector_remove_at);
    RUNTEST(vector_reserve);
    RUNTEST(vector_resize);

    PRINTTESTSUITE(Vector);

    ENDTEST();
}
