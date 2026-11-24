add_library(ihft_types INTERFACE)
target_include_directories(ihft_types INTERFACE include)

target_link_libraries(ihft_types INTERFACE ihft_compiler)

add_subdirectory(test)
add_subdirectory(benchmark)
