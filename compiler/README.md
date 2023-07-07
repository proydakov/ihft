# ifth::compiler

This module contains various macros for detailed control of the compilation process.

`COLD` marco is useful for lambda expressions or functions containing error handling code. This allows you to remove code that is rare in execution from the hot processing path and exclude it from the decoding process.

```cpp
IHFT_COLD
```

`NOINLINE` macro is useful for benchmarks and other code in which we want to keep the structure of the code breakdown by methods written by the programmer.

```cpp
IHFT_NOINLINE
```
