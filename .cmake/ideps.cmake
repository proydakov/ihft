if (IHFT_BUILD_UNITTESTS)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.13.6
    )

    FetchContent_GetProperties(Catch2)
    if(NOT Catch2_POPULATED)
        FetchContent_Populate(Catch2)
        #message(STATUS "${catch2_SOURCE_DIR} - ${catch2_BINARY_DIR}")
        add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    endif()
endif()

if (IHFT_BUILD_TOML)
    include(FetchContent)

    FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG        2cee913b6d627a964caae0e3f03ac9ad1c7afed1
    )

    FetchContent_GetProperties(tomlplusplus)
    if(NOT tomlplusplus_POPULATED)
        FetchContent_Populate(tomlplusplus)
        #message(STATUS "${tomlplusplus_SOURCE_DIR} - ${tomlplusplus_BINARY_DIR}")
        add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR})
    endif()
endif()
