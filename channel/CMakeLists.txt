add_library(ihft_channel INTERFACE)
target_include_directories(ihft_channel INTERFACE include)
target_link_libraries(ihft_channel INTERFACE ihft_constant)

add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(measure)
