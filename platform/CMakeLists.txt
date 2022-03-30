add_library(ihft_platform src/platform.cpp src/cmdline.cpp src/process_cpu_list.cpp)
target_include_directories(ihft_platform PUBLIC include)
target_link_libraries(ihft_platform PRIVATE ihft_constant)

add_subdirectory(test)
add_subdirectory(example)
