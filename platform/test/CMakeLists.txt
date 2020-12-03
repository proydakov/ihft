# unit test apps
ihft_add_test(test_cpu_isolation)
target_link_libraries(test_cpu_isolation ihft_platform)

ihft_add_test(test_platform)
target_link_libraries(test_platform ihft_platform)
