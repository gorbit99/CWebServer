set(SRC_FILES main.c)

add_subdirectory(http)
add_subdirectory(tests)
add_subdirectory(utils)

add_executable(server "${SRC_FILES}")

target_link_libraries(server PRIVATE http tests utils)

set_target_properties(server
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/release"
    EXPORT_COMPILE_COMMANDS ${GENERATE_COMPILE_COMMANDS}
    C_STANDARD 11
    )

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(server PRIVATE -g ${REQUIRED_COMPILE_FLAGS})
endif()

add_executable(test EXCLUDE_FROM_ALL "${SRC_FILES}")
target_compile_definitions(test PRIVATE TESTING)

set_target_properties(test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
    C_STANDARD 11
    )

target_compile_options(test PRIVATE -g ${REQUIRED_COMPILE_FLAGS})

target_link_libraries(test PRIVATE http tests utils)
