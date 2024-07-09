ihft_add_test(test_sso)

ihft_add_test(test_result)
target_link_libraries(test_result PRIVATE ihft_types)

ihft_add_test(test_function_ref)
target_link_libraries(test_function_ref PRIVATE ihft_types)

ihft_add_test(test_box)
target_link_libraries(test_box PRIVATE ihft_types)

ihft_add_test(test_scope_exit)
target_link_libraries(test_scope_exit PRIVATE ihft_types)
