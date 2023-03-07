# manual example apps
add_executable(channel_echo echo.cpp)
target_link_libraries(channel_echo ihft_channel)

add_executable(channel_ping_pong ping_pong.cpp)
target_link_libraries(channel_ping_pong ihft_channel ihft_constant)
