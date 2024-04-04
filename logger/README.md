# ihft::logger

## Concept

Logging helps us to monitor the behavior of the code with minimal overhead. The following steps occur during data logging:

- The current thread copies input arguments into logger_event object with a fixed internal storage for dynamic data
- The executor thread is trying to quickly push a pointer to the logger_event event to the logging queue of this thread
- A separate processing thread pops the event and performs its formatting, the finalized event is dumped to disk or to the network, the logger_event destructor is called.

Briefly, this scheme can be described by the sequence:

- Fast copy input.
- Push in queue.
- Format & notify.

The design is based on the following requirements:

- No system calls are made during the process of creating and pushing events.
- Writing to disk or network is performed on a separate thread.
- All threads log independently and do not compete for memory.

The following picture illustrates the flow:

![initial](/.image/logger.png)

## Limitations

An important limitation of the system design is that if the logging queue overflows on a particular thread, events are discarded. Thus the overloaded logging system does not guarantee that all events will be delivered.
If compile-time estimated event size is above logger_event::ITEM_SIZE threshold a compilation error is thrown.
Events containing dynamic data could be trimmed to fit into the internal fixed storage.
The trimming policy is set via logger_contract<> template class specialization.

## Quick start

One can use the logging system in a fairly simple way:

```
    IHFT_LOG_INFO("Hello {} {}!!!", "world", 1024);
```

As a result of executing this code, the following line will appear in the log:

```
    UTC 2024-07-06 08:58:05.518553  INFO [main:1818552] logger_simple.cpp(15:42):'int main()' Hello world 1024 !!!
```

The formatted message provides the following information:

- The time of sending the event.
- Event logging level (Debug|Info|Warning|Error).
- Name and identifier of the sender's thread.
- The file, line and position that performed the logging.
- The name of the function that performed the logging.
- The text of the event transmitted by the user.

## Custom types support

Template class logger_contract<> specialization should be provided to log data of a custom type.

See a full source of [logger_contract](include/logger/logger_contract.h).

## Examples

[logger_simple example](example/logger_simple.cpp)

[logger_mthreads example](example/logger_mthreads.cpp)

## Benchmarks

[logger_construct benchmark](benchmark/benchmark_logger_construct.cpp)
