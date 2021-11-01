#define TYPE       int
#define NAME       int
#define STRUCTNAME Int
#define DECLARE_VECTOR
#define IMPLEMENT_VECTOR
#include "Vector.h"

TEST(vector_new) {
    STARTTEST();

    VectorInt *vector = vector_int_new();

    TESTASSERT(The default size of the vector is 0, vector->size == 0);
    TESTASSERT(The default capacity of the vector is the defined one,
               vector->capacity == 5);

    vector_int_free(vector);

    ENDTEST();
}

TEST(vector_push_back_and_get) {
    STARTTEST();

    VectorInt *vector = vector_int_new();
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);

    TESTASSERT(The size of the vector is 7, vector->size == 7);
    TESTASSERT(The capacity of the vector is 10, vector->capacity == 10);

    TESTASSERT(The first element is 5, *vector_int_get(vector, 0) == 5);
    TESTASSERT(The fourth element is 7, *vector_int_get(vector, 3) == 7);

    vector_int_free(vector);

    ENDTEST();
}

TEST(vector_insert) {
    STARTTEST();

    VectorInt *vector = vector_int_new();
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    int elem4 = 10;
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);
    vector_int_insert(vector, &elem4, 2);

    TESTASSERT(The size of the vector is 4, vector->size == 4);
    TESTASSERT(The capacity of the vector is 5, vector->capacity == 5);

    TESTASSERT(The first element is 5, *vector_int_get(vector, 0) == 5);
    TESTASSERT(The third element is 10, *vector_int_get(vector, 2) == 10);

    vector_int_free(vector);

    ENDTEST();
}

TEST(vector_pop_back) {
    STARTTEST();

    VectorInt *vector = vector_int_new();

    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);

    vector_int_pop_back(vector);

    TESTASSERT(The size of the vector is 2, vector->size == 2);

    vector_int_free(vector);

    ENDTEST();
}

TEST(vector_remove_at) {
    STARTTEST();

    VectorInt *vector = vector_int_new();

    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem1);

    vector_int_remove_at(vector, 2);

    TESTASSERT(The size of the vector is 4, vector->size == 4);
    TESTASSERT(The second element is 7, *vector_int_get(vector, 1) == 7);
    TESTASSERT(The third element is 7, *vector_int_get(vector, 2) == 7);

    vector_int_free(vector);

    ENDTEST();
}

TEST(vector_reserve) {
    STARTTEST();

    VectorInt *vector = vector_int_new();
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);

    vector_int_reserve(vector, 2);
    TESTASSERT(If the capacity is smaller than the current one do nothing,
               vector->capacity == 5);

    vector_int_reserve(vector, 20);
    TESTASSERT(Otherwise increase capacity, vector->capacity == 20);
    TESTASSERT(Dont touch the size, vector->size == 3);

    vector_int_free(vector);

    ENDTEST();
}

TEST(vector_resize) {
    STARTTEST();

    VectorInt *vector = vector_int_new();
    int elem1 = 5;
    int elem2 = 7;
    int elem3 = 6;
    vector_int_push_back(vector, &elem1);
    vector_int_push_back(vector, &elem2);
    vector_int_push_back(vector, &elem3);

    int filler = 54;
    vector_int_resize(vector, 10, &filler);

    TESTASSERT(The size should be 10, vector->size == 10);
    for (size_t i = 3; i < 10; ++i) {
        TESTASSERT(Every new element is 54, *vector_int_get(vector, i) == 54);
    }

    vector_int_free(vector);

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
