# ifth::platform

This module provides a bridge for working with the application startup system.

The main class is [ihft::platform::trait](include/platform/platform.h).

```
namespace ihft::platform
{

struct trait
```

It contains a lot of methods for detecting system configuration or change some runtime parameters.

## Examples

[Command line set_thread_cpu example](example/set_thread_cpu.cpp)

[Command line set_thread_name example](example/set_thread_name.cpp)

[Command line get_platform_info example](example/get_platform_info.cpp)

[Command line core_2_core_latancy example](example/core_2_core_latancy.cpp)

[Command line sysjitter example](example/sysjitter.cpp)

[Command line bogatyr example](example/bogatyr.cpp)

[Command line greek_alphabet example](example/greek_alphabet.cpp)

[Command line china_cities example](example/china_cities.cpp)
