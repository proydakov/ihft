# ihft::timer

This module contains code with a low-level function for manipulating time.

`cpu_counter` provides an access for native core tick counter.

```cpp
namespace ihft::timer
{

inline unsigned long long cpu_counter() // aka RDTSC

}
```

`cpu_pause` allows to sleep a several cpu cycles without enter in os mode.

```cpp
namespace ihft::timer
{

inline void cpu_pause() // aka PAUSE

}
```

## Benchmarks

[timer benchmark](benchmark/benchmark_timer.cpp)
