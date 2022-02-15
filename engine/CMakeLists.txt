add_library(ihft_engine src/logical_cpu.cpp src/cpus_configuration.cpp)
target_include_directories(ihft_engine PUBLIC include)
target_link_libraries(ihft_engine PUBLIC ihft_types PRIVATE ihft_platform)

add_subdirectory(test)
add_subdirectory(example)
