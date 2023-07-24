# ihft::misc

This module contains an utility code for *ihft* applications.

TOML-based configuration class [config_helper](include/misc/config_helper.h)

```cpp
namespace ihft::misc
{

// This class provides a simple config assist
// Configuration data is immutable after loading
class config_helper final
```

Unix singnal helper with several methods [signal_helper](include/misc/signal_helper.h)

```cpp
namespace ihft::misc
{

using sa_sigaction_t = void (*)(int, siginfo_t*, void*);

bool setup_sigaction_handler(sa_sigaction_t, std::initializer_list<int> signals, std::optional<int> flags = std::nullopt);

bool block_application_signals(std::initializer_list<int> signals);
bool block_thread_signals(std::initializer_list<int> signals);
```

## Examples

[sigaction_demo example](example/sigaction_demo.cpp)

[config_helper example](example/config_helper_demo.cpp)
