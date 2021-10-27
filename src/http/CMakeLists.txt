add_library(http STATIC
    EXCLUDE_FROM_ALL
    headers.h headers.c
    request.h request.c
    response.h response.c
    response_codes.h response_codes.c
    socket.h
    socket/socket_linux.c
    verb.h verb.c
    version.h version.c)

set_target_properties(http
    PROPERTIES
    C_STANDARD 11
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/release"
    )

target_link_libraries(http PRIVATE utils)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(http PRIVATE -g ${REQUIRED_COMPILE_FLAGS})
endif()
