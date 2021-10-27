add_library(tests STATIC
    EXCLUDE_FROM_ALL
    framework.h framework.c
    )

set_target_properties(tests
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/release"
    C_STANDARD 11
    )

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(tests PRIVATE -g ${REQUIRED_COMPILE_FLAGS})
endif()
