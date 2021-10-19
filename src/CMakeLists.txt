set(SRC_FILES 
    main.c
    tests/framework.h tests/framework.c
    utils/Optional.h utils/Optional.c
    )

add_executable(server "${SRC_FILES}")

set_target_properties(server
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "./bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "./bin/release"
    RUNTIME_OUTPUT_DIRECTORY_TEST "./bin/test"
    EXPORT_COMPILE_COMMANDS ${GENERATE_COMPILE_COMMANDS}
    )

if ("${MASOCHISTIC_COMPILATION}" AND "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(server 
        PRIVATE 
        -Wall -Wextra -Wpedantic -Werror)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "TEST")
    target_compile_definitions(server PRIVATE TESTING)
endif()
