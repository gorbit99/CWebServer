#pragma once

#include "../tests/framework.h"

#include <stddef.h>

#define vector_new(type) _vector_new_base(sizeof(type))

typedef struct Vector Vector;

Vector *_vector_new_base(size_t data_size);

void vector_free(Vector *vector);

void vector_push_back(Vector *vector, void *elem);

void vector_insert(Vector *vector, void *elem, size_t index);

void vector_pop_back(Vector *vector);

void vector_remove_at(Vector *vector, size_t index);

void *vector_get(Vector *vector, size_t index);

size_t vector_size(Vector *vector);

void vector_reserve(Vector *vector, size_t capacity);

void vector_resize(Vector *vector, size_t size, void *default_value);

TEST(vector);
