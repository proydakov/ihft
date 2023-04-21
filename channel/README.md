# ihft::channel

> Do not communicate by sharing memory; instead, share memory by communicating.

## Concept

This module contains code for working with channels. Channels help us transfer data from one thread to another in real time. The IHFT implementation of channels focuses on low latency, reliability and ease of working with code, as far as it is possible to express it in C++. Scalability is sacrificed. The current implementation uses fixed-size heap array as base for circular buffer. There is potentially an implementation on linked lists, which allows to change the capacity at runtime, however, this implementation will slow down the writer's thread in case of a resize.

## Types

The module contains an implementation for four types of channels:

- one writer to one reader (aka single producer single consumer) for POD structs.
- one writer to one reader (aka single producer single consumer) for complex moveable classes with RAII support.
- one writer to many readers (aka single producer multiple consumer) for POD structs.
- one writer to many readers (aka single producer multiple consumer) for complex moveable classes with RAII support.

## Quick start

Working with channels begins with the creation of a pair `{writer, readers}` using the factory.

```cpp
channel_factory::make<QUEUE_TYPE>(capacity, readers_count) -> optional{producer, consumers}

channel_factory::make<QUEUE_TYPE>(capacity, readers_count, content_allocator) -> optional{producer, consumers}
```

After successful creation, the user can start message passing. The developer can use reader and writer objects in arbitrary threads. A queue is represented on a user side by two objects - "reader" and "writer" which are movable only". Each writer and reader contains a shared pointer to a associated ring buffer. It will be thread-safe destroyed when the last writer and reader is destroyed.

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

During the experiment with latency calculating, the writer thread sends the current high-resolution timestamp to the ring buffer and performs an operation to synchronize the cache memory between cores `fense`. By itself, writing to the queue does not lead to a `fense` call, this happens only in a specific testing scenario. At the same time, the reader's thread(s) receive events with a timestamp, get the current high-resolution timestamp and determine the difference between receiving and sending. Readers use the technique of calculating the maximum latency per interval to minimize memory access. [The full source code of measure](measure/data_latency.h). The test program uses thread binding to isolated processors.

The test machine configuration:

```
~$ uname --all
Linux pro-X570-AORUS-ULTRA 5.15.0-58-generic #64-Ubuntu SMP Thu Jan 5 11:43:13 UTC 2023 x86_64 x86_64 x86_64 GNU/Linux

~$ cat /proc/cmdline
BOOT_IMAGE=/boot/vmlinuz-5.15.0-58-generic ro quiet splash transparent_hugepage=never default_hugepagesz=1G hugepagesz=1G isolcpus=6-11 nohz_full=6-11 rcu_nocbs=6-11 vt.handoff=7

~$ cpu-info
Packages:
    0: AMD Ryzen 9 5900X
Microarchitectures:
    12x Zen 3
Cores:
    0: 1 processor (0), AMD Zen 3
    1: 1 processor (1), AMD Zen 3
    2: 1 processor (2), AMD Zen 3
    3: 1 processor (3), AMD Zen 3
    4: 1 processor (4), AMD Zen 3
    5: 1 processor (5), AMD Zen 3
    6: 1 processor (6), AMD Zen 3
    7: 1 processor (7), AMD Zen 3
    8: 1 processor (8), AMD Zen 3
    9: 1 processor (9), AMD Zen 3
    10: 1 processor (10), AMD Zen 3
    11: 1 processor (11), AMD Zen 3
Logical processors (System ID):
    0 (0): APIC ID 0x00000000
    1 (1): APIC ID 0x00000001
    2 (2): APIC ID 0x00000002
    3 (3): APIC ID 0x00000003
    4 (4): APIC ID 0x00000004
    5 (5): APIC ID 0x00000005
    6 (6): APIC ID 0x00000008
    7 (7): APIC ID 0x00000009
    8 (8): APIC ID 0x0000000a
    9 (9): APIC ID 0x0000000b
    10 (10): APIC ID 0x0000000c
    11 (11): APIC ID 0x0000000d
```

Experiment: Single producer and single consumer.

```
./measure_one2one_stream_pod_queue_latency 6,7 512
usage: ./measure_one2one_stream_pod_queue_latency <cpu-list or #> <total_events> * 10^6 <queue_capacity>
```

| Reader ID | Text report | Hist |
| --- | --- | --- |
| reader 1 | samples: 16000000<br>percentile[50]: 80us<br>percentile[75]: 80us<br>percentile[80]: 80us<br>percentile[95]: 90us<br>percentile[99]: 90us<br>percentile[99.9]: 650us<br>percentile[100]: 6160us | ![img](/.image/channel_one2one.jpeg) |

Experiment: Single producer and two consumers.

```
./measure_one2many_stream_pod_queue_latency 6,7,8 2 512
usage: ./measure_one2many_stream_pod_queue_latency <cpu-list or #> <num_readers> <total_events> * 10^6 <queue_capacity>
```

| Reader ID | Text report | Hist |
| --- | --- | --- |
| reader 1 | samples: 16000000<br>percentile[50]: 130us<br>percentile[75]: 140us<br>percentile[80]: 140us<br>percentile[95]: 140us<br>percentile[99]: 160us<br>percentile[99.9]: 216074us<br>percentile[100]: 223755us | ![img](/.image/channel_one2many2_r0.jpeg) |
| reader 2 | samples: 16000000<br>percentile[50]: 130us<br>percentile[75]: 140us<br>percentile[80]: 140us<br>percentile[95]: 150us<br>percentile[99]: 160us<br>percentile[99.9]: 216034us<br>percentile[100]: 223745us | ![img](/.image/channel_one2many2_r1.jpeg) |

Experiment: Single producer and three consumers.

```
/measure_one2many_stream_pod_queue_latency 6,7,8,9 3 512
usage: ./measure_one2many_stream_pod_queue_latency <cpu-list or #> <num_readers> <total_events> * 10^6 <queue_capacity>
```

| Reader ID | Text report | Hist |
| --- | --- | --- |
| reader 1 | samples: 16000000<br>percentile[50]: 170us<br>percentile[75]: 190us<br>percentile[80]: 200us<br>percentile[95]: 200us<br>percentile[99]: 210us<br>percentile[99.9]: 840us<br>percentile[100]: 225365us | ![img](/.image/channel_one2many3_r0.jpeg) |
| reader 2 | samples: 16000000<br>percentile[50]: 180us<br>percentile[75]: 190us<br>percentile[80]: 200us<br>percentile[95]: 200us<br>percentile[99]: 220us<br>percentile[99.9]: 810us<br>percentile[100]: 215755us | ![img](/.image/channel_one2many3_r1.jpeg) |
| reader 3 | samples: 16000000<br>percentile[50]: 180us<br>percentile[75]: 190us<br>percentile[80]: 200us<br>percentile[95]: 200us<br>percentile[99]: 220us<br>percentile[99.9]: 840us<br>percentile[100]: 225385us | ![img](/.image/channel_one2many3_r2.jpeg) |

## Examples

[echo example](example/echo.cpp)

[ping_pong example](example/ping_pong.cpp)
