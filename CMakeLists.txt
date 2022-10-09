cmake_minimum_required(VERSION 3.12)

project(fail2abuseipdb VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "A simple utility for converting fail2ban jail info to abuseipdb csv format")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS ON) # optional

# Set options for submodules
# I don't want to build Nils' tests

set(JSON_BuildTests OFF CACHE BOOL "Disable JSON test utils")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/json)

# Set compiler options
add_compile_options(
    -Wpedantic
    -Werror
    -Wall
)

file(GLOB_RECURSE FILES FOLLOW_SYMLINKS ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

include_directories(
    include/
)

add_executable(
    ${PROJECT_NAME}

    ${FILES}
)

target_link_libraries(
    ${PROJECT_NAME}

    fmt
    nlohmann_json
)