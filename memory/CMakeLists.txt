add_library(ihft_memory INTERFACE)
target_include_directories(ihft_memory INTERFACE include)
target_link_libraries(ihft_memory INTERFACE ihft_constant)

add_subdirectory(test)
add_subdirectory(benchmark)
