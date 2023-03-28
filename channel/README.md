# ifth::channel

> Do not communicate by sharing memory; instead, share memory by communicating.

## Concept

This module contains code for working with channels. Channels help us transfer data from one processor to another in real time. The IHFT implementation of channels focuses on low latency, reliability and ease of working with code, as far as it is possible to express it in C++. Scalability is sacrificed. The current implementation uses fixed-size heap array as base for circular buffer. There is potentially an implementation on linked lists, which allows to change the capacity at runtime, however, this implementation will slow down the writer's processor in case of a resize.

## Types

The module contains an implementation for four types of channels:

- one writer to one reader (aka spsc) for pod structs.
- one writer to one reader (aka spsc) for complex moveable classes with RAII support.
- one writer to many readers (aka spmc) for pod structs.
- one writer to many readers (aka spmc) for complex moveable classes with RAII support.

## Quick start

Working with channels begins with the creation of a pair `{writer, readers}` using the factory.

```cpp
channel_factory::make<Q>(capacity, readers_count) -> opt{producer, consumers}

channel_factory::make<Q>(capacity, readers_count, content_allocator) -> opt{producer, consumers}
```

After successful creation, the user can start message passing. The developer can use the writer and the reader both in the same thread and in different ones. Also user should use `std::move()` with `writer` and `reader` to transfer ownership between threads. The sharing a reference on writer or reader between threads are prohibited. Such an attempt to write/read will lead to undefined behavior. Each writer and reader contains a shared pointer to a ring buffer. It will be thread-safe destroyed when the last writer and reader is destroyed.

A typical usage scenario:

```cpp
make_writer_thread([producer = std::move(producer)](){
    ...
    auto data = make_data();
    producer.try_write(std::move(data));
    ...
});

make_reader_thread([consumer = std::move(consumer)](){
    ...
    auto opt = consumer.try_read();
    if (opt)
    {
        auto& data_ref = *opt;
    }
    ...
});
```

## How it works

Let's look at the state of a single producer single consumer cyclic buffer with a capacity of 16 in various scenarios.

| Ring buffer state | Description |
| --- | --- |
| ![initial](/.image/channel_initial.jpeg) | Initial state. The ring buffer is empty. The writer and reader sequence number is 0. All cells of the cyclic buffer contain the service value of the sequence number, which means that there is no data in the cell yet. |
| ![somedata](/.image/channel_somedata.jpeg) | The ring buffer contains 3 elem. The writer sequence number is 3, the reader requence number still 0. The reader can read the data three times using the `try_read()` method. |
| ![nodata](/.image/channel_nodata.jpeg) | The ring buffer is empty. The writer and reader sequence number is 3. If `try_read()` is called, the reader will receive an empty optional `std::nullopt`. |
| ![full](/.image/channel_full.jpeg) | The ring buffer is full. The writer call `try_write()` will return `false`. The reader can read the data of `capacity()` attempts. The writer sequence number is 19, the reader sequence number is 3. |

### Latency

During the experiment with latency calculating, the writer thread sends the current high-resolution timestamp to the ring buffer and performs an operation to synchronize the cache memory between cores `fense`. At the same time, the reader's thread(s) receive events with a timestamp, get the current high-resolution timestamp and determine the difference between receiving and sending. Readers uses the technique of calculating the maximum latency per interval to minimize memory access. [The full source code of measure.](measure/data_latency.h)

Experiment: Single producer and single consumer.

| Reader ID | Text report | Hist |
| --- | --- | --- |
| reader 1 | file: reader_0<br>samples: 16000000<br>percentile[50]: 80<br>percentile[75]: 80<br>percentile[80]: 80<br>percentile[95]: 90<br>percentile[99]: 90<br>percentile[99.9]: 650<br>percentile[100]: 6160 | ![img](/.image/channel_one2one.jpeg) |

Experiment: Single producer and two consumers.

| Reader ID | Text report | Hist |
| --- | --- | --- |
| reader 1 | file: reader_0<br>samples: 16000000<br>percentile[50]: 130<br>percentile[75]: 140<br>percentile[80]: 140<br>percentile[95]: 140<br>percentile[99]: 160<br>percentile[99.9]: 216074<br>percentile[100]: 223755 | ![img](/.image/channel_one2many2_r0.jpeg) |
| reader 1 | file: reader_1<br>samples: 16000000<br>percentile[50]: 130<br>percentile[75]: 140<br>percentile[80]: 140<br>percentile[95]: 150<br>percentile[99]: 160<br>percentile[99.9]: 216034<br>percentile[100]: 223745 | ![img](/.image/channel_one2many2_r1.jpeg) |

Experiment: Single producer and three consumers.

| Reader ID | Text report | Hist |
| --- | --- | --- |
| reader 1 | file: reader_0<br>samples: 16000000<br>percentile[50]: 170<br>percentile[75]: 190<br>percentile[80]: 200<br>percentile[95]: 200<br>percentile[99]: 210<br>percentile[99.9]: 840<br>percentile[100]: 225365 | ![img](/.image/channel_one2many3_r0.jpeg) |
| reader 2 | file: reader_1<br>samples: 16000000<br>percentile[50]: 180<br>percentile[75]: 190<br>percentile[80]: 200<br>percentile[95]: 200<br>percentile[99]: 220<br>percentile[99.9]: 810<br>percentile[100]: 215755 | ![img](/.image/channel_one2many3_r1.jpeg) |
| reader 3 | file: reader_2<br>samples: 16000000<br>percentile[50]: 180<br>percentile[75]: 190<br>percentile[80]: 200<br>percentile[95]: 200<br>percentile[99]: 220<br>percentile[99.9]: 840<br>percentile[100]: 225385 | ![img](/.image/channel_one2many3_r2.jpeg) |

## Examples

[echo example](example/echo.cpp)

[ping_pong example](example/ping_pong.cpp)
