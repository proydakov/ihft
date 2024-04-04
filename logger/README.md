# ihft::logger

## Concept

This module contains code for logging system.

Logging helps us monitor the behavior of the code with minimal overhead. The following steps occur during data logging:

- the current thread copies input arguments into logger_event object with a fixed internal storage for dynamic data
- the executor thread is trying to quickly send a pointer to the logger_event event to the logging queue of this thread
- a separate processing thread subtracts the event and performs its formatting, the finished event is dumped to disk or to the network, the logger_event destructor is called.

Briefly, this scheme can be described by the sequence:

- fast copy input
- send in queue
- format & notify

The design is based on the following requirements:

- no system calls are made during the process of creating and sending events
- writing to disk or network is performed on a separate thread
- all threads log independently and do not compete for memory among themselves

In the picture below you can clearly see all these components:

![initial](/.image/channel_initial.jpeg)

## Limitations

An important limitation of the system design is that if the logging queue overflows on a particular thread, events are discarded. This means that the overload logging system does not guarantee 100% delivery of all events.
If the logged event is too large and the compiler can calculate it, a compilation error will be thrown.
If the logged event contains dynamic data and they will not fit into the internal fixed storage, then they will be trimmed, the trimming mechanism is set by the user in the specification of the `logger_contract` class for a specific type.

## Quick start

You can use the logging system in a fairly simple way:

```
    IHFT_LOG_INFO("Hello {} {}!!!", "world", 1024);
```

As a result of executing this code, the following line will appear in the log:

```
    UTC 2024-07-06 08:58:05.518553  INFO [main:1818552] logger_simple.cpp(15:42):'int main()' Hello world 1024 !!!
```

In the formatted message, we can see the following information:

- the time of sending the event
- event logging level (Debug|Info|Warning|Error)
- name and identifier of the sender's thread
- the file, line and position that performed the logging
- the name of the function that performed the logging
- the text of the event transmitted by the user

## Custom types support

For implementing a custom type support, you should specialize logger_contract<> for your type.

See a full source of [logger_contract](example/logger_contract.cpp).

## Examples

[logger_simple example](example/logger_simple.cpp)

[logger_mthreads example](example/logger_mthreads.cpp)

## Benchmarks

[logger_construct benchmark](benchmark/benchmark_logger_construct.cpp)
