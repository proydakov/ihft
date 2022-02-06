# manual example apps

if(IHFT_BUILD_TOML)
    add_executable(toml_demo toml_demo.cpp)
    target_link_libraries(toml_demo tomlplusplus::tomlplusplus)
endif()

add_executable(misc_config_helper_demo config_helper_demo.cpp)
target_link_libraries(misc_config_helper_demo ihft_misc)

add_executable(sigaction_demo sigaction_demo.cpp)
