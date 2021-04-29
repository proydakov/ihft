#==============================================================================
# Copyright (c) 2020 Evgeny Proydakov <lord.tiran@gmail.com>
#==============================================================================

cmake_minimum_required(VERSION 3.12.0)
project(IHFT)

message(STATUS "create IHFT")

include(.cmake/icmake.cmake)

enable_testing()

add_subdirectory(compiler)
add_subdirectory(constant)
add_subdirectory(misc)
add_subdirectory(memory)
add_subdirectory(channel)
add_subdirectory(platform)

ihft_build_report()
