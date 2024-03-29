option(IHFT_LINK_STATIC "Use static runtime for linkage. Compile dependency free executables." OFF)
option(IHFT_LINK_STRIP  "Strip binaries in linker stage." OFF)
option(IHFT_MARCH_NATIVE "Using -march=native for code generation." OFF)
option(IHFT_SANITIZER_ADDRESS "Using address sanitizers for project." OFF)
option(IHFT_SANITIZER_THREAD "Using thread sanitizers for project." OFF)
option(IHFT_SANITIZER_UB "Using undefined behavior sanitizer for project." OFF)
option(IHFT_BUILD_UNITTESTS "Build unit tests for project." ON)
option(IHFT_BUILD_TOML "Build tomlplusplus library for parsing configurations." ON)
option(IHFT_PERF_PROFILING "Use -fno-omit-frame-pointer compile flag." ON)
