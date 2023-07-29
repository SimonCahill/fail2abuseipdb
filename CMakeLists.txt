cmake_minimum_required(VERSION 3.12)

project(fail2abuseipdb VERSION 0.2.0 LANGUAGES CXX DESCRIPTION "A simple utility for converting fail2ban jail info to abuseipdb csv format")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS ON) # optional
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp)

###
# Ensure all submodules are initialised
###
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/git_submodule.cmake)

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

include_directories(
    include/
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

if (DEFINED fail2abuseipdb_DEBUG)
    message(STATUS "### ENABLED DEBUG ###")
    add_compile_options(
        -O0
        -ggdb
    )
else()
    add_compile_options(
        -O2
        -s
    )
endif()

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

###
# Docs target
###
add_custom_target("docs" COMMENT "Create Doxygen documentation")
add_custom_command(
    TARGET "docs"
    POST_BUILD
        COMMENT "Generate Doxygen documentation for publication or reading"
        COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

###
# Install and Remove targets and their configs go here
###
set(fail2abuseipdb_INSTALL_DIR "/usr/local/bin")

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${fail2abuseipdb_INSTALL_DIR}
)

add_custom_target("uninstall" COMMENT "Uninstall fail2abuseipdb")
add_custom_command(
    TARGET "uninstall"
    POST_BUILD
        COMMENT "Uninstall fail2abuseipdb from the current system"
        COMMAND xargs rm -Rfv < install_manifest.txt || echo "Nothing to be removed from system!"
)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CONTACT "Simon Cahill <simon@simonc.eu>")
set(CPACK_PACKAGE_VENDOR "Simon Cahill")
set(CPACK_GENERATOR DEB)
include(CPack)