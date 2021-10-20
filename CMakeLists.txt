cmake_minimum_required(VERSION 3.15)

project(WebServer
    LANGUAGES C
    DESCRIPTION "An http webserver written entirely in C99"
    VERSION 0.1.0)


if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
endif()

set(ALLOWED_BUILD_TYPES DEBUG RELEASE)
list(JOIN ALLOWED_BUILD_TYPES ", " ALLOWED_BUILD_TYPES_STRING)

if (NOT "${CMAKE_BUILD_TYPE}" IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR 
        "Build type needs to be one of:"
        "${ALLOWED_BUILD_TYPES_STRING}")
endif()

option(GENERATE_COMPILE_COMMANDS
    "Generate compile_commands.json file" 
    ON)
option(MASOCHISTIC_COMPILATION 
    "Compile with -Werror -Wall -Wextra -Wpedantic" 
    ON)

add_subdirectory("src")
