message(STATUS "Downloading dependencies...")

if(${CMAKE_VERSION} VERSION_GREATER "3.23")
    cmake_policy(SET CMP0135 OLD)
endif()

include(FetchContent)
#set(FETCHCONTENT_QUIET FALSE)

# catch2
if (IHFT_BUILD_UNITTESTS)
    FetchContent_Declare(
        Catch2
        URL https://codeload.github.com/catchorg/Catch2/tar.gz/refs/tags/v3.6.0
        URL_HASH MD5=86a9fec7afecaec687faaa988ac6818e
    )
    FetchContent_GetProperties(Catch2)
    if(NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        #message(STATUS "${catch2_SOURCE_DIR} - ${catch2_BINARY_DIR}")
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()

    target_compile_options(Catch2 PRIVATE -Wno-sign-conversion)
    target_compile_options(Catch2 PRIVATE -Wno-unused-parameter)
    target_compile_options(Catch2 PRIVATE -Wno-conversion)
    target_compile_options(Catch2 PRIVATE -Wno-implicit-int-float-conversion)

    add_library(catch2_test_main OBJECT ${PROJECT_SOURCE_DIR}/.cmake/template/catch2_test_main.cpp)
    target_link_libraries(catch2_test_main PUBLIC Catch2::Catch2WithMain)

    add_library(catch2_benchmark_main OBJECT ${PROJECT_SOURCE_DIR}/.cmake/template/catch2_benchmark_main.cpp)
    target_link_libraries(catch2_benchmark_main PUBLIC Catch2::Catch2WithMain)
endif()

# toml++
if (IHFT_BUILD_TOML)
    FetchContent_Declare(
        tomlplusplus
        URL https://codeload.github.com/marzer/tomlplusplus/tar.gz/refs/tags/v3.3.0
        URL_HASH MD5=a50999cb3ec1b05fd4ba535b5fee5e62
    )
    FetchContent_GetProperties(tomlplusplus)
    if(NOT tomlplusplus_POPULATED)
        FetchContent_Populate(tomlplusplus)
        #message(STATUS "${tomlplusplus_SOURCE_DIR} - ${tomlplusplus_BINARY_DIR}")
        add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR})
    endif()
endif()
