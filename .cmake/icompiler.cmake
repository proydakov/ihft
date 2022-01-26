macro(ihft_compiler_set_max_warning_level)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -pedantic -pedantic-errors -Wunused -Wconversion -Wsign-conversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic -pedantic-errors -Wunused -Wconversion -Wsign-conversion")
endmacro()

###############################################################################

macro(ihft_setup_compiler_flags)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS 0)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    else()
        message(FATAL_ERROR "Unknown compiler")
    endif()

    if(IHFT_MARCH_NATIVE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

    if(NOT IHFT_STATIC_LINK)
        set(IHFT_SANITIZER_OPT "-ggdb -fno-omit-frame-pointer")
        if(IHFT_SANITIZER_ADDRESS AND IHFT_SANITIZER_UB)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=address -fsanitize=undefined")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=address -fsanitize=undefined")
        elseif(IHFT_SANITIZER_ADDRESS)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=address")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=address")
        elseif(IHFT_SANITIZER_THREAD)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=thread")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=thread")
        elseif(IHFT_SANITIZER_UB)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=undefined")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IHFT_SANITIZER_OPT} -fsanitize=undefined")
        endif()
    endif()

    ihft_compiler_set_max_warning_level()
endmacro()

###############################################################################

macro(ihft_setup_tools_flags)
    # colors =)
    set (CMAKE_BUILD_COLOR_MAKEFILE ON)

    if(CMAKE_GENERATOR STREQUAL "Ninja")
        # Turn on colored output. https://github.com/ninja-build/ninja/wiki/FAQ
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
        endif()
    endif()
endmacro()
