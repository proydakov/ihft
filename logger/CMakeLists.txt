add_library(ihft_logger src/logger.cpp)
target_include_directories(ihft_logger PUBLIC include)
target_link_libraries(ihft_logger PUBLIC ihft_types)

add_subdirectory(test)
