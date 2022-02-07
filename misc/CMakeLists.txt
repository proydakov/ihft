add_library(ihft_misc src/config_helper.cpp)
target_include_directories(ihft_misc PUBLIC include)
target_link_libraries(ihft_misc PUBLIC ihft_types PRIVATE ihft_constant)
if(IHFT_BUILD_TOML)
    target_link_libraries(ihft_misc PRIVATE tomlplusplus::tomlplusplus)
endif()

add_subdirectory(test)
add_subdirectory(example)
