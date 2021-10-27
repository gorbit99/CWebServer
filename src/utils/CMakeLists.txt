add_library(utils STATIC
    EXCLUDE_FROM_ALL
    Errors.h Errors.c
    Hashmap.h Hashmap.c
    Optional.h Optional.c
    string.h string.c
    Vector.h Vector.c
    )

target_link_libraries(utils PRIVATE tests)

set_target_properties(utils
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/release"
    C_STANDARD 11
    )

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(utils PRIVATE -g ${REQUIRED_COMPILE_FLAGS})
endif()
