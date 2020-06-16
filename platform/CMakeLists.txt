add_library(ihft_platform src/platform.cpp)
target_include_directories(ihft_platform PUBLIC include)

add_subdirectory(test)

