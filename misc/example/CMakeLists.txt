# manual example apps

if(IHFT_BUILD_TOML)
    add_executable(toml_demo toml_demo.cpp)
    target_link_libraries(toml_demo tomlplusplus::tomlplusplus)
endif()
