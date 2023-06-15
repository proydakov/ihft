add_library(ihft_timer INTERFACE)
target_include_directories(ihft_timer INTERFACE include)

add_subdirectory(example)
add_subdirectory(benchmark)
