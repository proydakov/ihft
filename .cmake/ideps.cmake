message(STATUS "Downloading dependencies...")

include(FetchContent)
#set(FETCHCONTENT_QUIET FALSE)

# catch2
if (IHFT_BUILD_UNITTESTS)
    FetchContent_Declare(
        Catch2
        URL https://codeload.github.com/catchorg/Catch2/tar.gz/refs/tags/v2.13.10
        URL_HASH MD5=4fe03ff55a8f25b0e093fbc964992ffa
    )
    FetchContent_GetProperties(Catch2)
    if(NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        #message(STATUS "${catch2_SOURCE_DIR} - ${catch2_BINARY_DIR}")
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()

    add_library(catch2_test_main OBJECT ${PROJECT_SOURCE_DIR}/.cmake/template/catch2_test_main.cpp)
    target_link_libraries(catch2_test_main PUBLIC Catch2::Catch2)

    add_library(catch2_benchmark_main OBJECT ${PROJECT_SOURCE_DIR}/.cmake/template/catch2_benchmark_main.cpp)
    target_link_libraries(catch2_benchmark_main PUBLIC Catch2::Catch2)
endif()

# toml++
if (IHFT_BUILD_TOML)
    FetchContent_Declare(
        tomlplusplus
        URL https://codeload.github.com/marzer/tomlplusplus/tar.gz/refs/tags/v3.3.0
        URL_HASH MD5=0a39749cc3881cadeebd0c1baac472c8
    )
    FetchContent_GetProperties(tomlplusplus)
    if(NOT tomlplusplus_POPULATED)
        FetchContent_Populate(tomlplusplus)
        #message(STATUS "${tomlplusplus_SOURCE_DIR} - ${tomlplusplus_BINARY_DIR}")
        add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR})
    endif()
endif()
