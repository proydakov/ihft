add_library(ihft_engine src/engine.cpp src/engine_main.cpp)
target_include_directories(ihft_engine PUBLIC include)
target_link_libraries(ihft_engine PUBLIC ihft_types PRIVATE ihft_platform ihft_misc ihft_logger)

add_subdirectory(test)
add_subdirectory(example)
