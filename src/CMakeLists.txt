set(SRC_FILES 
    main.c
    tests/framework.h tests/framework.c
    utils/Optional.h utils/Optional.c
    utils/Vector.h utils/Vector.c
    utils/Hashmap.h utils/Hashmap.c
    utils/Errors.h utils/Errors.c
    http/socket.h
    http/socket/socket_linux.c
    )

add_executable(server "${SRC_FILES}")

set_target_properties(server
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release"
    EXPORT_COMPILE_COMMANDS ${GENERATE_COMPILE_COMMANDS}
    )

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(server PRIVATE -g)

    if ("${MASOCHISTIC_COMPILATION}")
        target_compile_options(server 
            PRIVATE 
            -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

add_executable(test EXCLUDE_FROM_ALL "${SRC_FILES}")
target_compile_definitions(test PRIVATE TESTING)

set_target_properties(test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
    )

target_compile_options(test PRIVATE -g)
if ("${MASOCHISTIC_COMPILATION}")
    target_compile_options(test 
        PRIVATE 
        -Wall -Wextra -Wpedantic -Werror)
endif()
