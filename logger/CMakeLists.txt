add_library(ihft_logger
    src/logger_adapter.cpp
    src/logger_client.cpp
    src/logger_level.cpp
    src/logger_listener.cpp
    src/default_logger_listener.cpp
)

target_include_directories(ihft_logger PUBLIC include)
target_link_libraries(ihft_logger PUBLIC ihft_constant)
target_link_libraries(ihft_logger PRIVATE ihft_channel)
target_link_libraries(ihft_logger PRIVATE ihft_types)
target_link_libraries(ihft_logger PRIVATE ihft_memory)
target_link_libraries(ihft_logger PRIVATE ihft_platform)

add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(benchmark)
