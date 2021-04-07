# auto test

ihft_add_test(test_stream_queue)
target_link_libraries(test_stream_queue PRIVATE ihft_channel)
