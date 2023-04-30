ihft_add_test(test_compiletime)

ihft_add_test(test_typer)

ihft_add_test(test_tuple_print)

ihft_add_test(test_tuple_format_accurately)

ihft_add_test(test_tuple_format_every)

ihft_add_test(test_source_location)
target_link_libraries(test_source_location PRIVATE ihft_logger)

ihft_add_test(test_logger_event)
target_link_libraries(test_logger_event PRIVATE ihft_logger)

