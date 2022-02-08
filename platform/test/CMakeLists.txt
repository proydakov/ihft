# unit test apps
ihft_add_test(test_cmdline)
target_link_libraries(test_cmdline PRIVATE ihft_types)
target_link_libraries(test_cmdline PRIVATE ihft_platform)

ihft_add_test(test_platform)
target_link_libraries(test_platform PRIVATE ihft_platform)
