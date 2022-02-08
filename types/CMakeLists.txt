add_library(ihft_types INTERFACE)
target_include_directories(ihft_types INTERFACE include)

add_subdirectory(test)
add_subdirectory(benchmark)
