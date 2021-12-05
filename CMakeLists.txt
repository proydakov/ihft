#==============================================================================
# Copyright (c) 2020 Evgeny Proydakov <lord.tiran@gmail.com>
#==============================================================================

# Offline mode:
# cmake -DFETCHCONTENT_UPDATES_DISCONNECTED=ON ..
# More details here
# https://stackoverflow.com/questions/67408357/how-to-avoid-update-checks-with-cmake-fetchcontent

cmake_minimum_required(VERSION 3.12.0)
project(IHFT)

message(STATUS "create IHFT")

include(.cmake/icmake.cmake)

enable_testing()

add_subdirectory(compiler)
add_subdirectory(constant)
add_subdirectory(misc)
add_subdirectory(memory)
add_subdirectory(network)
add_subdirectory(channel)
add_subdirectory(platform)
add_subdirectory(engine)

ihft_build_report()
