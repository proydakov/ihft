# auto test

ihft_add_test(test_one2one_stream_queue)
target_link_libraries(test_one2one_stream_queue PRIVATE ihft_channel)
target_link_libraries(test_one2one_stream_queue PRIVATE Catch2::Catch2)

ihft_add_test(test_one2many_stream_queue)
target_link_libraries(test_one2many_stream_queue PRIVATE ihft_channel)
target_link_libraries(test_one2many_stream_queue PRIVATE Catch2::Catch2)

# manual unit tests

# test main library

add_library(ihft_queue_test_main INTERFACE)
target_include_directories(ihft_queue_test_main INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ihft_queue_test_main INTERFACE ihft_channel)
target_link_libraries(ihft_queue_test_main INTERFACE ihft_platform)

# one2many_stream

add_executable(channel_one2many_stream_object_queue_perf_plain
    plain_data.h
    test_one2many_stream_object_queue_plain.cpp
)
target_link_libraries(channel_one2many_stream_object_queue_perf_plain ihft_queue_test_main)

add_executable(channel_one2many_stream_object_queue_perf_heap
    heap_data.h
    test_one2many_stream_object_queue_heap.cpp
)
target_link_libraries(channel_one2many_stream_object_queue_perf_heap ihft_queue_test_main)

add_executable(channel_one2many_stream_object_queue_latency
    latency.h
    test_one2many_stream_object_queue_latency.cpp
)
target_link_libraries(channel_one2many_stream_object_queue_latency ihft_queue_test_main)

add_executable(channel_one2many_stream_pod_queue_perf_plain
    plain_data.h
    test_one2many_stream_pod_queue_plain.cpp
)
target_link_libraries(channel_one2many_stream_pod_queue_perf_plain ihft_queue_test_main)

add_executable(channel_one2many_stream_pod_queue_latency
    latency.h
    test_one2many_stream_pod_queue_latency.cpp
)
target_link_libraries(channel_one2many_stream_pod_queue_latency ihft_queue_test_main)

# one2one_stream

add_executable(channel_one2one_stream_object_queue_perf_plain
    plain_data.h
    test_one2one_stream_object_queue_plain.cpp
)
target_link_libraries(channel_one2one_stream_object_queue_perf_plain ihft_queue_test_main)

add_executable(channel_one2one_stream_object_queue_perf_heap
    plain_data.h
    test_one2one_stream_object_queue_heap.cpp
)
target_link_libraries(channel_one2one_stream_object_queue_perf_heap ihft_queue_test_main)

add_executable(channel_one2one_stream_object_queue_latency
    plain_data.h
    test_one2one_stream_object_queue_latency.cpp
)
target_link_libraries(channel_one2one_stream_object_queue_latency ihft_queue_test_main)

add_executable(channel_one2one_stream_pod_queue_perf_plain
    plain_data.h
    test_one2one_stream_pod_queue_plain.cpp
)
target_link_libraries(channel_one2one_stream_pod_queue_perf_plain ihft_queue_test_main)

add_executable(channel_one2one_stream_pod_queue_latency
    plain_data.h
    test_one2one_stream_pod_queue_latency.cpp
)
target_link_libraries(channel_one2one_stream_pod_queue_latency ihft_queue_test_main)
