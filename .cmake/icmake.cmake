# BEGIN CMAKE PREPARATION

# setup cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/.cmake)

include(ios)

if(NOT ((PROJECT_OS_LINUX OR PROJECT_OS_OSX) AND PROJECT_PROC_64BIT))
    message(FATAL_ERROR "Only Linux/MacOS amd64 cpu supported.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    #set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(BUILD_SHARED_LIBS OFF)

include(ioption)
include(imacro)
include(icompiler)
include(ilinker)
include(ideps)

ihft_setup_compiler_flags()
ihft_setup_linker_flags()
ihft_setup_tools_flags()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

if (IHFT_BUILD_UNITTESTS)
    add_library(catch2_test_main OBJECT ${PROJECT_SOURCE_DIR}/.cmake/template/catch2_test_main.cpp)
    target_link_libraries(catch2_test_main PUBLIC Catch2::Catch2)

    add_library(catch2_benchmark_main OBJECT ${PROJECT_SOURCE_DIR}/.cmake/template/catch2_benchmark_main.cpp)
    target_link_libraries(catch2_benchmark_main PUBLIC Catch2::Catch2)
endif()

# END CMAKE PREPARATION
