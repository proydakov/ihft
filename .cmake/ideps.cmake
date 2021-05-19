message(STATUS "Downloading dependencies...")

include(FetchContent)
#set(FETCHCONTENT_QUIET FALSE)

if (IHFT_BUILD_UNITTESTS)
    FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v2.13.6.tar.gz
        URL_HASH MD5=c7c7ef181b9e08418fd9f2ef8159d03f
    )
    FetchContent_GetProperties(Catch2)
    if(NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        #message(STATUS "${catch2_SOURCE_DIR} - ${catch2_BINARY_DIR}")
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()
endif()

if (IHFT_BUILD_TOML)
    FetchContent_Declare(
        tomlplusplus
        URL https://github.com/marzer/tomlplusplus/archive/refs/tags/v2.4.0.tar.gz
        URL_HASH MD5=546d163e5f37d9bbdc5e19702aaaac8f
    )
    FetchContent_GetProperties(tomlplusplus)
    if(NOT tomlplusplus_POPULATED)
        FetchContent_Populate(tomlplusplus)
        #message(STATUS "${tomlplusplus_SOURCE_DIR} - ${tomlplusplus_BINARY_DIR}")
        add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR})
    endif()
endif()
