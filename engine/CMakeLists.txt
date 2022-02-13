add_library(ihft_engine src/logical_cpu.cpp)
target_include_directories(ihft_engine PUBLIC include)
target_link_libraries(ihft_engine PRIVATE ihft_platform)

add_subdirectory(test)
add_subdirectory(example)
