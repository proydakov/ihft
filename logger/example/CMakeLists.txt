# manual example apps
add_executable(logger_mthreads logger_mthreads.cpp)
target_link_libraries(logger_mthreads ihft_logger ihft_platform ihft_timer)

add_executable(logger_simple logger_simple.cpp)
target_link_libraries(logger_simple ihft_logger)
