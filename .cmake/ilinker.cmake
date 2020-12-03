macro(ihft_setup_linker_flags)
    string(REPLACE "." ";" COMPILER_VERSION_LIST ${CMAKE_CXX_COMPILER_VERSION})
    list(GET COMPILER_VERSION_LIST 0 MAJOR_CXX_VERSION)

    find_program(LLD_PATH NAMES "lld-${MAJOR_CXX_VERSION}" "lld")
    find_program(GOLD_PATH NAMES "gold")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CXX_RUNTIME_LIBRARY "libstdc++")
        if(GOLD_PATH)
            set(LINKER_NAME "gold")
            set(CMAKE_LINKER ${GOLD_PATH})
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CXX_RUNTIME_LIBRARY "libstdc++")
        if(LLD_PATH)
            set(LINKER_NAME "lld")
            set(CMAKE_LINKER ${LLD_PATH})
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(CXX_RUNTIME_LIBRARY "libc++")
        if(LLD_PATH)
            set(LINKER_NAME "lld")
            set(CMAKE_LINKER ${LLD_PATH})
        endif()
    else()
        message(FATAL_ERROR "Unknown compiler")
    endif()

    # try to use libc++ runtime for clang
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        execute_process(COMMAND readlink -fn ${CMAKE_C_COMPILER} OUTPUT_VARIABLE TEST_CLANG_ROOT)
        get_filename_component(TEST_CLANG_ROOT ${TEST_CLANG_ROOT} DIRECTORY)
        get_filename_component(TEST_CLANG_ROOT ${TEST_CLANG_ROOT} DIRECTORY)

        if(EXISTS "${TEST_CLANG_ROOT}/include/c++/v1/" AND
           EXISTS "${TEST_CLANG_ROOT}/lib/libc++.a" AND
           EXISTS "${TEST_CLANG_ROOT}/lib/libc++abi.a" AND
           EXISTS "${TEST_CLANG_ROOT}/lib/libc++.so" AND
           EXISTS "${TEST_CLANG_ROOT}/lib/libc++abi.so")

            message(STATUS "Detected clang root: ${TEST_CLANG_ROOT}")
            set(CLANG_ROOT ${TEST_CLANG_ROOT})

            add_compile_options(-nostdinc++ -I${CLANG_ROOT}/include/c++/v1)
            #add_compile_options(--sysroot ${MUSL_ROOT} -nostdinc -I${MUSL_ROOT}/include -nostdinc++ -I${CLANG_ROOT}/include/c++/v1)
            if(IHFT_STATIC_LINK)
                set(CLANG_EXTRA_LINKS "-stdlib=libc++ -lpthread -lc++abi -L${CLANG_ROOT}/lib")
                # manual link list for static glibc & libc++ & libc++abi
                #set(CLANG_EXTRA_LINKS "-nodefaultlibs -lpthread -lc -lm -lgcc -lgcc_eh -lc++ -lc++abi -L${CLANG_ROOT}/lib")
                # manual link list for static musl & libc++ & libc++abi
                #set(CLANG_EXTRA_LINKS "-nodefaultlibs --sysroot ${MUSL_ROOT} -stdlib=libc++ -lc -lc++ -lc++abi -lunwind -L${CLANG_ROOT}/lib")
            else()
                set(CLANG_EXTRA_LINKS "-stdlib=libc++ -lpthread -lc++abi -L${CLANG_ROOT}/lib -Wl,-rpath,${CLANG_ROOT}/lib")
            endif()
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CLANG_EXTRA_LINKS}")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CLANG_EXTRA_LINKS}")
            set(CXX_RUNTIME_LIBRARY "libc++")
        else()
            message(FATAL_ERROR "Please install libc++ & libc++abi for compilation")
        endif()
    endif()

    if(CXX_RUNTIME_LIBRARY STREQUAL "libstdc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")

        if(IHFT_STATIC_LINK)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
        else()
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lpthread")
        endif()
    endif()

    if(IHFT_STATIC_LINK)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared")
    endif()

    if(IHFT_STRIP_LINK)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
    endif()

    if(LINKER_NAME)
        message(STATUS "Using linker: ${LINKER_NAME} (selected from: LLD_PATH=${LLD_PATH}; GOLD_PATH=${GOLD_PATH})")
        set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=${LINKER_NAME}${CMAKE_EXE_LINKER_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=${LINKER_NAME}${CMAKE_SHARED_LINKER_FLAGS}")
    else()
        message(STATUS "Use default linker")
    endif()
endmacro()
