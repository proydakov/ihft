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
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include(ioption)
include(icompiler)
include(ilinker)

ihft_setup_compiler_flags()
ihft_setup_linker_flags()
ihft_setup_tools_flags()

include(ideps)
include(imacro)

if(NOT IHFT_BUILD_TOML)
    message(FATAL_ERROR "Building without toml is not supported at the moment.")
endif()
