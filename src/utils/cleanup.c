#include "cleanup.h"

void cleanup_string(String **data) {
    string_free(*data);
}
