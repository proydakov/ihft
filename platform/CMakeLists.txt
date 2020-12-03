add_library(ihft_platform src/platform.cpp src/isolation.cpp)
target_include_directories(ihft_platform PUBLIC include)

add_subdirectory(test)
add_subdirectory(example)
