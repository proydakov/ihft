# ifth::platform

This module provides a bridge for working with the application startup system.

The main class is [ihft::platform::trait](include/platform/platform.h).

```cpp
namespace ihft::platform
{

struct trait
```

It contains a lot of methods for detecting system configuration or change some runtime parameters.

## Examples

[set_thread_cpu example](example/set_thread_cpu.cpp)

[set_thread_name example](example/set_thread_name.cpp)

[get_platform_info example](example/get_platform_info.cpp)

[core_2_core_latancy example](example/core_2_core_latancy.cpp)

[sysjitter example](example/sysjitter.cpp)

[bogatyr example](example/bogatyr.cpp)

[greek_alphabet example](example/greek_alphabet.cpp)

[china_cities example](example/china_cities.cpp)
