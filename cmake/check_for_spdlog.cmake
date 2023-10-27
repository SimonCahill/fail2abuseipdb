##################################################
##  Simple CMake script which checks for the    ##
##  presence of libspdlog on the system/in      ##
##  the library path and downloads the required ##
##  version if necessary.                       ##
##                                              ##
##  Written by Simon Cahill                     ##
##################################################

if (NOT TARGET spdlog)
    include(FetchContent)

    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG 9797b353c0c116160b6181ce36cff1b59c4958c7
    )

    FetchContent_MakeAvailable(spdlog)
    set(f2abuseipdb_PROTO_LIB "spdlog")
else()
    set(f2abuseipdb_PROTO_LIB "spdlog")
endif()
