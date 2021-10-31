#pragma once

#include "../tests/framework.h"

#include <stddef.h>

#define vector_new(type) _vector_new_base(sizeof(type))

#define vector_get(vector, index, type) \
    *(type *)(_vector_get_base(vector, index))

typedef struct Vector Vector;

Vector *_vector_new_base(size_t data_size);

void vector_free(Vector *vector);

void vector_push_back(Vector *vector, void *elem);

void vector_insert(Vector *vector, void *elem, size_t index);

void vector_pop_back(Vector *vector);

void vector_remove_at(Vector *vector, size_t index);

void *_vector_get_base(Vector *vector, size_t index);

size_t vector_size(Vector *vector);

void vector_reserve(Vector *vector, size_t capacity);

void vector_resize(Vector *vector, size_t size, void *default_value);

void vector_foreach(Vector *vector, void (*func)(void *));

void vector_foreach_with_data(Vector *vector,
                              void (*func)(void *data, void *userdata),
                              void *userdata);

TEST(vector);
