# ifth::engine

This module contains a main function for *ihft* applications.

```cpp
namespace ihft::engine
{
    class cpus_config;
    class task_storage;

    using register_tasks_callback_t = types::function_ref<bool(cpus_config const&, task_storage&, std::atomic_bool const&)>;
    using invalid_config_callback_t = types::function_ref<void()>;

    int engine_main(int const argc, char const * const argv[], register_tasks_callback_t, invalid_config_callback_t);
}
```

It provides a comprehensive functionality for:

- parse and validate an application configuration
- validation of the application launch platform
- complex signal handler initialization
- thread initialization and processor bindings
- binding user tasks to execution threads
- waiting for the end of the application

Demonstration in Linux (htop):

![perimeter](/.image/engine_perimeter.png)

## Examples

[engine_perimeter example](example/engine_perimeter.cpp)

[engine_manual_config example](example/engine_manual_config.cpp)
