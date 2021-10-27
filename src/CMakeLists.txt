set(SRC_FILES 
    main.c
    tests/framework.h tests/framework.c
    utils/Optional.h utils/Optional.c
    utils/Vector.h utils/Vector.c
    utils/Hashmap.h utils/Hashmap.c
    utils/Errors.h utils/Errors.c
    utils/string.h utils/string.c
    http/socket.h
    http/socket/socket_linux.c
    http/request.h http/request.c
    http/version.h http/version.c
    http/verb.h http/verb.c
    http/response.h http/response.c
    http/headers.h http/headers.c
    http/response_codes.h http/response_codes.c
    )

add_executable(server "${SRC_FILES}")

set_target_properties(server
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release"
    EXPORT_COMPILE_COMMANDS ${GENERATE_COMPILE_COMMANDS}
    C_STANDARD 11
    )

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(server PRIVATE -g)

    if ("${MASOCHISTIC_COMPILATION}")
        target_compile_options(server 
            PRIVATE 
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wconversion
            -Wold-style-definition
            -Wdouble-promotion
            -Wformat=2
            -Wformat-overflow=2
            -Wformat-signedness
            -Wimplicit-fallthrough=2
            -Wswitch-default
            -Wshift-overflow=2
            -Wstringop-overflow=4
            -Wsuggest-attribute=pure
            -Wsuggest-attribute=const
            -Wmissing-noreturn
            -Wsuggest-attribute=malloc
            -Wmissing-format-attribute
            -Wsuggest-attribute=cold
            -Wduplicated-branches
            -Wduplicated-cond
            -Wfloat-equal
            -Wbad-function-cast
            -Wcast-qual
            -Wlogical-op
            -Wredundant-decls
            -Wnested-externs
            -Wunsuffixed-float-constants
            )
    endif()
endif()

add_executable(test EXCLUDE_FROM_ALL "${SRC_FILES}")
target_compile_definitions(test PRIVATE TESTING)

set_target_properties(test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
    C_STANDARD 11
    )

target_compile_options(test PRIVATE -g)
if ("${MASOCHISTIC_COMPILATION}")
    target_compile_options(test 
        PRIVATE 
        -Wall -Wextra -Wpedantic -Werror)
endif()
