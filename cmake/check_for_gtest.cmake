##################################################
##  Simple CMake script which checks for the    ##
##  presence of gtest on the system/in          ##
##  the library path and downloads the required ##
##  version if necessary.                       ##
##                                              ##
##  Not written by Simon Cahill                 ##
##################################################

if (NOT TARGET gtest)
    include(FetchContent)

    FetchContent_Declare(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
    )

    FetchContent_MakeAvailable(gtest)
    set(f2abuseipdb_PROTO_LIB "gtest")
else()
    set(f2abuseipdb_PROTO_LIB "gtest")
endif()
