###############################################################################
# Copyright (c) 2020 Evgeny Proydakov <lord.tiran@gmail.com>
###############################################################################

function(ihft_build_report)

set(BUILD_INFO_BAR "====================================================================================")
set(NOOP_STRING "")

set(RUNTIME_MODE "shared")
if(IHFT_STATIC_LINK)
    set(RUNTIME_MODE "static")
endif()

message(STATUS ${BUILD_INFO_BAR})
message(STATUS "Summary of the build:")
message(STATUS ${BUILD_INFO_BAR})
message(STATUS "CMAKE:        ${CMAKE_COMMAND} [${CMAKE_VERSION}]")
message(STATUS "GENERATOR:    ${CMAKE_GENERATOR}")
if(BUILD_TOOL_VERSION)
message(STATUS "BUILD_TOOL:   ${CMAKE_BUILD_TOOL} [${BUILD_TOOL_VERSION}]")
else()
message(STATUS "BUILD_TOOL:   ${CMAKE_BUILD_TOOL}")
endif()
message(STATUS "C_COMPILER:   ${CMAKE_C_COMPILER} [${CMAKE_C_COMPILER_ID}-${CMAKE_C_COMPILER_VERSION}]")
message(STATUS "CXX_COMPILER: ${CMAKE_CXX_COMPILER} [${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}]")
message(STATUS "LINKER:       ${CMAKE_LINKER}")
message(STATUS "RUNTIME MODE: ${RUNTIME_MODE}")
message(STATUS "CXX_RUNTIME:  ${CXX_RUNTIME_LIBRARY}")
message(STATUS "")
message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "CMAKE_C_FLAGS:   ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG:   ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "")
message(STATUS "CMAKE_EXE_LINKER_FLAGS:    ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS ${BUILD_INFO_BAR})
message(STATUS ${NOOP_STRING})

endfunction()

function(ihft_add_test NAME)
    add_executable(${NAME} ${NAME}.cpp)
    add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()
