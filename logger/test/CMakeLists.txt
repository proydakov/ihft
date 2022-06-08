ihft_add_test(test_compiletime)

ihft_add_test(test_typer)

ihft_add_test(test_tuple_print)

ihft_add_test(test_serialize)
target_link_libraries(test_serialize PRIVATE ihft_constant)
