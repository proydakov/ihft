# ihft::compiler

This module contains various macros for detailed control of the compilation process.

`COLD` marco is useful for lambda expressions or functions containing error handling code. This allows you to remove code that is rare in execution from the hot processing path and exclude it from the decoding process.

```cpp
    [](unsigned cpu) IHFT_COLD
    {
        std::cerr << ("Invalid cpu processor: " + std::to_string(cpu) + "\n");
    }(cpu);
```

`NOINLINE` macro is useful for benchmarks and other code in which we want to keep the structure of the code breakdown by methods written by the programmer. It could be usefull for perf profiling to better flamegraph reports.

```cpp
    IHFT_NOINLINE bool char_cmp(char c1, char c2)
    {
        return c1 == c2;
    }
```
