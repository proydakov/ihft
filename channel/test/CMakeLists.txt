include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(one2many_bitmask_queue_perf_plain
    main.h
    test_plain_data.h
    test_bitmask_queue_plain_data.cpp
)
target_link_libraries(one2many_bitmask_queue_perf_plain ihft_channel)
target_link_libraries(one2many_bitmask_queue_perf_plain ihft_platform)

add_executable(one2many_bitmask_queue_perf_heap
    main.h
    test_heap_data.h
    test_bitmask_queue_heap_data.cpp
)
target_link_libraries(one2many_bitmask_queue_perf_heap ihft_channel)
target_link_libraries(one2many_bitmask_queue_perf_heap ihft_platform)

add_executable(one2many_bitmask_queue_latency
    main.h
    test_latency.h
    test_bitmask_queue_latency.cpp
)
target_link_libraries(one2many_bitmask_queue_latency ihft_channel)
target_link_libraries(one2many_bitmask_queue_latency ihft_platform)

add_executable(one2many_counter_queue_perf_plain
    main.h
    test_plain_data.h
    test_counter_queue_plain_data.cpp
)
target_link_libraries(one2many_counter_queue_perf_plain ihft_channel)
target_link_libraries(one2many_counter_queue_perf_plain ihft_platform)

add_executable(one2many_counter_queue_perf_heap
    main.h
    test_heap_data.h
    test_counter_queue_heap_data.cpp
)
target_link_libraries(one2many_counter_queue_perf_heap ihft_channel)
target_link_libraries(one2many_counter_queue_perf_heap ihft_platform)

add_executable(one2many_counter_queue_latency
    main.h
    test_latency.h
    test_counter_queue_latency.cpp
)
target_link_libraries(one2many_counter_queue_latency ihft_channel)
target_link_libraries(one2many_counter_queue_latency ihft_platform)
