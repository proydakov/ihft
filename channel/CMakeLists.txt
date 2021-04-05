add_library(ihft_channel INTERFACE)
target_include_directories(ihft_channel INTERFACE include)

add_subdirectory(test)
add_subdirectory(benchmark)