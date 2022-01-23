message(STATUS "Downloading dependencies...")

include(FetchContent)
#set(FETCHCONTENT_QUIET FALSE)

if (IHFT_BUILD_UNITTESTS)
    FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v2.13.7.tar.gz
        URL_HASH MD5=933e58313f752d142f915698e720e467
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
        URL https://codeload.github.com/marzer/tomlplusplus/tar.gz/refs/tags/v3.0.1
        URL_HASH MD5=cb20c01fb297032f5232249fb1b180fb
    )
    FetchContent_GetProperties(tomlplusplus)
    if(NOT tomlplusplus_POPULATED)
        FetchContent_Populate(tomlplusplus)
        #message(STATUS "${tomlplusplus_SOURCE_DIR} - ${tomlplusplus_BINARY_DIR}")
        add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR})
    endif()
endif()
