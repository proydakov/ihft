#pragma once

#include <ratio>
#include <atomic>
#include <string>
#include <bitset>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include <platform/platform.h>
#include <channel/channel_factory.h>

#include <x86intrin.h>

using namespace ihft;

#define NOINLINE  __attribute__((noinline))

struct alignas(platform::CPU_CACHE_LINE_SIZE) wait_t
{
    long waitCounter = 0;
};

template<typename reader_t, typename controller_t>
long NOINLINE reader_method_impl(std::size_t total_events, reader_t& reader, controller_t& controller)
{
    long waitCounter = 0;

    for (std::size_t j = 0; j < total_events;)
    {
        auto opt = reader.try_read();
        if (opt)
        {
            controller.check_data(reader.get_id(), *opt);
            j++;
        }
        else
        {
            waitCounter++;
            //_mm_pause();
        }
    }

    return waitCounter;
}

template<typename reader_t, typename controller_t>
void reader_method(std::size_t total_events, reader_t reader, wait_t& stat, std::atomic<std::uint64_t>& waitinig_readers_counter, controller_t& controller)
{
    auto const name = "reader_" + std::to_string(reader.get_id());
    platform::set_current_thread_name(name.c_str());

    waitinig_readers_counter--;

    stat.waitCounter = reader_method_impl(total_events, reader, controller);

    controller.reader_done();
}

template<typename queue_t, typename controller_t>
long NOINLINE writer_method_impl(std::size_t total_events, queue_t& queue, controller_t& controller)
{
    long waitCounter = 0;

    for (std::size_t j = 0; j < total_events;)
    {
        auto data = controller.create_data(j);

label:

        if(queue.try_write(std::move(data), std::memory_order_seq_cst))
        {
            j++;
        }
        else
        {
            waitCounter++;
            //_mm_pause();

            goto label;
        }
    }

    return waitCounter;
}

template<typename queue_t, typename controller_t>
void writer_method(std::size_t total_events, queue_t& queue, wait_t& stat, std::atomic<std::uint64_t>& waitinig_readers_counter, controller_t& controller)
{
    platform::set_current_thread_name("writer");

    while(waitinig_readers_counter > 0);

    stat.waitCounter = writer_method_impl(total_events, queue, controller);

    controller.writer_done();
}

// SFINAE test
template <typename T>
class has_get_content_allocator
{
    template <typename C> static std::true_type test( decltype(&C::get_content_allocator) );
    template <typename C> static std::false_type test( ... );

public:
    enum { value = std::is_same<decltype(test<T>(0)), std::true_type >::value };
};

template<typename C, typename Q>
auto make_controller(Q& queue, std::size_t NUM_READERS, std::size_t TOTAL_EVENTS)
{
    if constexpr(has_get_content_allocator<Q>::value)
    {
        return C{NUM_READERS, TOTAL_EVENTS, queue.get_content_allocator()};
    }
    else
    {
        return C{NUM_READERS, TOTAL_EVENTS};
    }
}

template<typename Q>
auto make_queue_with_readers(std::size_t QUEUE_SIZE, std::size_t NUM_READERS)
{
    if constexpr(has_get_content_allocator<Q>::value)
    {
        using A = typename Q::allocator_type;
        std::unique_ptr<A> allocator;
        if constexpr(std::is_constructible_v<A, std::size_t>)
        {
            allocator = std::make_unique<A>(QUEUE_SIZE);
        }
        else
        {
            allocator = std::make_unique<A>();
        }
        return channel_factory::make<Q>(QUEUE_SIZE, NUM_READERS, std::move(allocator));
    }
    else
    {
        return channel_factory::make<Q>(QUEUE_SIZE, NUM_READERS);
    }
}

template<typename Q, typename T, bool SINGLE_READER = false>
int test_main(int argc, char* argv[],
    std::uint64_t total_events = 64,
    std::uint64_t num_readers = std::max((std::thread::hardware_concurrency() - 1), 1u),
    std::uint64_t queue_capacity = 4096)
{
    std::cout << "usage: app <num readers> <events> * 10^6 <queue_capacity>" << std::endl;

    // TEST details
    auto const NUM_READERS = [&](){
        auto const readers = static_cast<std::size_t>(argc > 1 ? std::stoul(argv[1]) : num_readers);
        return SINGLE_READER ? 1 : readers;
    }();
    auto const TOTAL_EVENTS = static_cast<std::size_t>((argc > 2 ? std::stoul(argv[2]) : total_events) * std::mega::num);
    auto const QUEUE_CAPACITY = static_cast<std::size_t>(argc > 3 ? std::stoul(argv[3]) : queue_capacity);

    std::cout << "TEST: 1 writer, "
        << std::to_string(NUM_READERS) << " readers, "
        << std::to_string(TOTAL_EVENTS) << " total events, "
        << QUEUE_CAPACITY << " queue capacity"
        << std::endl;

    std::uint64_t rdtsc_start, rdtsc_end;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;

    wait_t writerWait;
    std::vector<wait_t> readersWait{ static_cast<std::size_t>(NUM_READERS) };

    {
        auto queue_with_readers = make_queue_with_readers<Q>(QUEUE_CAPACITY, NUM_READERS);
        if (not queue_with_readers)
        {
            std::cerr << "Can't create queue with: " << NUM_READERS << " reader." << std::endl;
            return EXIT_FAILURE;
        }
        auto& pair = *queue_with_readers;
        auto& queue = pair.producer;
        auto& readers = pair.consumers;

        T controller = make_controller<T>(queue, NUM_READERS, TOTAL_EVENTS);

        std::atomic<std::uint64_t> waitinig_readers_counter{ NUM_READERS };

        auto const mask = queue.readers_mask();
        std::cout << "alive mask: " << std::bitset<sizeof(mask) * 8>(mask) << " [" << mask << "]" << std::endl;;

        std::vector<std::thread> threads;
        threads.reserve(NUM_READERS);
        for (std::size_t i = 0; i < NUM_READERS; i++)
        {
            threads.emplace_back(reader_method<typename Q::reader_type, T>, TOTAL_EVENTS, std::move(readers[i]), std::ref(readersWait[i]), std::ref(waitinig_readers_counter), std::ref(controller));
        }

        {
            rdtsc_start = __rdtsc();
            start = std::chrono::high_resolution_clock::now();

            threads.emplace_back(writer_method<Q, T>, TOTAL_EVENTS, std::ref(queue), std::ref(writerWait), std::ref(waitinig_readers_counter), std::ref(controller));

            for (auto& t : threads) t.join();

            rdtsc_end = __rdtsc();
            stop = std::chrono::high_resolution_clock::now();
        }
    }

    auto const milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    auto const nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
    auto const rdtsc_delta = rdtsc_end - rdtsc_start;

    std::cout << "W WAIT: " << writerWait.waitCounter << "\n";
    for (auto const& stat : readersWait)
    {
        std::cout << "R WAIT: " << stat.waitCounter << "\n";
    }
    std::cout << "\n";
    std::cout << "TIME: " << milliseconds << " milliseconds\n";
    std::cout << "TIME: " << rdtsc_delta << " cycles\n";
    std::cout << "PERF: " << double(double(TOTAL_EVENTS) / double(milliseconds)) << " events/millisecond\n";
    std::cout << "PERF: " << double(double(nanoseconds) / double(TOTAL_EVENTS)) << " nanoseconds/event\n";
    std::cout << "PERF: " << double(double(rdtsc_delta) / double(TOTAL_EVENTS)) << " cycles/event\n";

    return 0;
}
