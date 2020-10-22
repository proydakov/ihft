# BEGIN CMAKE PREPARATION

# setup cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(idetectos)

if(NOT ((PROJECT_OS_LINUX OR PROJECT_OS_OSX) AND PROJECT_PROC_64BIT))
    message(FATAL_ERROR "Only Linux/MacOS x64bit supported.")
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

ihft_setup_compiler_flags()
ihft_setup_linker_flags()
ihft_setup_tools_flags()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# END CMAKE PREPARATION
