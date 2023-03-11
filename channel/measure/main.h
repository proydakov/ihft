#pragma once

#include <constant/constant.h>
#include <compiler/compiler.h>
#include <channel/channel_factory.h>
#include <platform/platform.h>
#include <platform/process_cpu_list.h>

#include <tuple>
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

#include <x86intrin.h>

using plat_t = ihft::platform::trait;
using cpus_t = std::vector<unsigned>;
using au64_t = std::atomic<std::uint64_t>;
using ou32_t = std::optional<std::uint32_t>;

struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) wait_t
{
    std::int64_t waitCounter = 0;
};

template<typename reader_t, typename controller_t>
std::int64_t IHFT_NOINLINE reader_method_impl(std::size_t total_events, reader_t& r, controller_t& c)
{
    std::int64_t waitCounter = 0;

    for (std::size_t j = 0; j < total_events;)
    {
        //_mm_lfence();
        auto opt = r.try_read();
        if (opt)
        {
            c.check_data(r.get_id(), j, *opt);
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

template<typename writer_t, typename controller_t>
std::int64_t IHFT_NOINLINE writer_method_impl(std::size_t total_events, writer_t& w, controller_t& c)
{
    std::int64_t waitCounter = 0;

    for (std::size_t j = 0; j < total_events; j++)
    {
        auto data = c.create_data(j);

        while (not w.try_write(std::move(data)))
        {
            waitCounter++;
            //_mm_pause();
        }

        if constexpr(controller_t::flush)
        {
            //_mm_mfence();
            //_mm_sfence();
            std::atomic_thread_fence(std::memory_order_seq_cst);
        }
    }

    return waitCounter;
}

template<bool SINGLE_READER>
bool help(int const argc, char const* argv[]);

template<bool SINGLE_READER>
auto parse_args(int const argc, char const* argv[]) -> std::tuple<cpus_t, size_t, size_t, size_t>;

void check_platform(const cpus_t& cpus);

template<typename Q>
auto make_queue(size_t queue_size, size_t num_readers);

template<typename C, typename Q>
auto make_controller(Q&, size_t num_readers, size_t total_events);

template<typename reader_t, typename controller_t>
void reader_method(reader_t, controller_t&, size_t total_events, au64_t& waitinig, wait_t&, ou32_t cpu);

template<typename writer_t, typename controller_t>
void writer_method(writer_t, controller_t&, size_t total_events, au64_t& waitinig, wait_t&, ou32_t cpu);

// main function for benchmark

template<typename Q, typename T, bool SINGLE_READER = false>
int test_main(int const argc, char const* argv[])
{
    if (help<SINGLE_READER>(argc, argv))
    {
        return EXIT_SUCCESS;
    }

    auto const& [cpus, num_readers, total_events, queue_request_capacity] = parse_args<SINGLE_READER>(argc, argv);

    if (not cpus.empty() and cpus.size() != (num_readers + 1))
    {
        std::cerr << "Cpu count [" << cpus.size() << "] != num readers + 1 [" << (num_readers + 1) << "]" << std::endl;
        return EXIT_FAILURE;
    }

    check_platform(cpus);

    namespace chrono = std::chrono;

    std::uint64_t rdtsc_start{}, rdtsc_end{};
    chrono::time_point<chrono::high_resolution_clock> start{}, stop{};

    wait_t writerWait;
    std::vector<wait_t> readersWait{ static_cast<std::size_t>(num_readers) };

    {
        auto queue_with_readers = make_queue<Q>(queue_request_capacity, num_readers);
        if (not queue_with_readers)
        {
            std::cerr << "Can't create queue with: " << num_readers << " reader." << std::endl;
            return EXIT_FAILURE;
        }

        auto& [writer, readers] = *queue_with_readers;

        std::cout << "MEASURE: 1 writer, "
            << std::to_string(num_readers) << " readers, "
            << std::to_string(total_events) << " total events, "
            << writer.capacity() << " queue capacity"
            << std::endl;

        T controller = make_controller<T>(writer, readers.size(), total_events);
        std::atomic<std::uint64_t> waitinig{ readers.size() };

        auto const mask = writer.readers_mask();
        std::cout << "alive mask: " << std::bitset<sizeof(mask) * 8>(mask) << " [" << mask << "]" << std::endl;

        std::vector<std::thread> threads;
        threads.reserve(readers.size());
        for (std::size_t i = 0; i < num_readers; i++)
        {
            threads.emplace_back(reader_method<typename Q::reader_type, T>,
                std::move(readers[i]),
                std::ref(controller),
                total_events,
                std::ref(waitinig),
                std::ref(readersWait[i]),
                not cpus.empty() ? std::make_optional(cpus[1 + i]) : std::nullopt
            );
        }

        {
            rdtsc_start = __rdtsc();
            start = chrono::high_resolution_clock::now();

            threads.emplace_back(writer_method<Q, T>,
                std::move(writer),
                std::ref(controller),
                total_events,
                std::ref(waitinig),
                std::ref(writerWait),
                not cpus.empty() ? std::make_optional(cpus[0]) : std::nullopt
            );

            for (auto& t : threads)
            {
                t.join();
            }

            rdtsc_end = __rdtsc();
            stop = chrono::high_resolution_clock::now();
        }
    }

    auto const format_events_per_second = [](std::size_t total_enents, std::int64_t nanos)
    {
        auto total_enents_per_micros = double(total_enents) / double(nanos);
        double base = total_enents_per_micros * double(1'000'000'000);
        std::size_t power = 0;
        while(base > 9)
        {
            base /= 10;
            power++;
        }
        std::string str = std::to_string(base);
        str.append(" * 10 ^ ").append(std::to_string(power));
        return str;
    };

    auto const calc_percent = [total = total_events](std::int64_t val)
    {
        std::string res;
        res.append("(");
        res.append(std::to_string(static_cast<double>(val) * 100.0 / double(total)));
        res.append("%)");
        return res;
    };

    auto const nanoseconds = chrono::duration_cast<chrono::nanoseconds>(stop - start).count();
    auto const rdtsc_delta = rdtsc_end - rdtsc_start;

    std::cout << "W WAIT: " << writerWait.waitCounter << ' ' << calc_percent(writerWait.waitCounter) << "\n";
    for (auto const& stat : readersWait)
    {
        std::cout << "R WAIT: " << stat.waitCounter << ' ' << calc_percent(stat.waitCounter) << "\n";
    }
    std::cout << "\n";

    std::cout << "TIME: " << double(nanoseconds) / double(1'000'000'000) << " seconds\n";
    std::cout << "TIME: " << rdtsc_delta << " cycles\n";
    std::cout << "PERF: " << format_events_per_second(total_events, nanoseconds) << " events/second\n";
    std::cout << "PERF: " << double(double(nanoseconds) / double(total_events)) << " nanoseconds/event\n";
    std::cout << "PERF: " << double(double(rdtsc_delta) / double(total_events)) << " cycles/event\n";

    return 0;
}

template<bool SINGLE_READER>
bool help(int const argc, char const* argv[])
{
    std::cout << "usage: " << argv[0] << " "
              << "<cpu-list or #> "
              << (SINGLE_READER ? "" : "<num_readers> ")
              << "<total_events> * 10^6 "
              << "<queue_capacity>"
              << "\n"
              << std::endl;

    if (argc == 2)
    {
        auto const a1 = std::string_view(argv[1]);
        if (a1 == "-h" || a1 == "-help" || a1 == "--help")
        {
            return true;
        }
    }

    return false;
}

template<bool SINGLE_READER>
auto parse_args(int const argc, char const* argv[]) -> std::tuple<cpus_t, size_t, size_t, size_t>
{
    // defaults
    std::uint64_t const total_events = 64;
    std::uint64_t const num_readers = std::max((std::thread::hardware_concurrency() - 1), 1u);
    std::uint64_t const queue_capacity = 4096;

    auto CPUS = [&]()
    {
        cpus_t result;

        // # is special mark. It means that all cpus are allowed
        auto const cpus = std::string_view(argc > 1 ? argv[1] : "#");

        if (std::string_view("#") != cpus)
        {
            ihft::platform::process_cpu_list(cpus, [&result](unsigned cpu) mutable
            {
                result.emplace_back(cpu);
            });
        }

        return result;
    }();

    auto NUM_READERS = [&]()
    {
        if (SINGLE_READER)
        {
            return 1ul;
        }
        else
        {
            auto const readers = static_cast<std::size_t>(argc > 2 ? std::stoul(argv[2]) : num_readers);
            return std::max(std::size_t{1}, readers);
        }
    }();

    auto TOTAL_EVENTS = [&]()
    {
        constexpr int teindex = SINGLE_READER ? 2 : 3;
        return static_cast<std::size_t>((argc > teindex ? std::stoul(argv[teindex]) : total_events) * std::mega::num);
    }();

    auto QUEUE_REQUEST_CAPACITY = [&]()
    {
        constexpr int qcindex = SINGLE_READER ? 3 : 4;
        return static_cast<std::size_t>(argc > qcindex ? std::stoul(argv[qcindex]) : queue_capacity);
    }();

    return {CPUS, NUM_READERS, TOTAL_EVENTS, QUEUE_REQUEST_CAPACITY};
}

void check_platform(const cpus_t& cpus)
{
    bool line{};

    if (not plat_t::is_scaling_governor_use_performance_mode())
    {
        std::cout << "WARNING. Benchmark started on cpu without performance mode.\n";
        line = true;
    }

    if (cpus.empty())
    {
        std::cout << "WARNING. Benchmark started without cpu list.\n";
        line = true;
    }
    else
    {
        for (auto const cpu : cpus)
        {
            if (not plat_t::get_cpu_isolation_status(cpu))
            {
                std::cout << "WARNING. cpu: " << cpu << " doesn't have system isolation.\n";
                line = true;
            }
        }
    }

    if (line)
    {
        std::cout << std::endl;
    }
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
auto make_controller(Q& queue, std::size_t num_readers, std::size_t total_events)
{
    if constexpr(has_get_content_allocator<Q>::value)
    {
        return C{num_readers, total_events, queue.get_content_allocator()};
    }
    else
    {
        return C{num_readers, total_events};
    }
}

template<typename Q>
auto make_queue(std::size_t queue_size, std::size_t num_readers)
{
    if constexpr(has_get_content_allocator<Q>::value)
    {
        using A = typename Q::allocator_type;
        std::unique_ptr<A> allocator;
        if constexpr(std::is_constructible_v<A, std::size_t>)
        {
            allocator = std::make_unique<A>(queue_size);
        }
        else
        {
            allocator = std::make_unique<A>();
        }
        return ihft::channel::channel_factory::make<Q>(queue_size, num_readers, std::move(allocator));
    }
    else
    {
        return ihft::channel::channel_factory::make<Q>(queue_size, num_readers);
    }
}

template<typename reader_t, typename controller_t>
void reader_method(reader_t r, controller_t& c, std::size_t total_events, au64_t& waitinig, wait_t& stat, ou32_t cpu)
{
    auto const name = "reader_" + std::to_string(r.get_id());
    plat_t::set_current_thread_name(name.c_str());
    if (cpu)
    {
        plat_t::set_current_thread_cpu(*cpu);
    }

    waitinig--;

    stat.waitCounter = reader_method_impl(total_events, r, c);

    c.reader_done();
}

template<typename writer_t, typename controller_t>
void writer_method(writer_t w, controller_t& c, std::size_t total_events, au64_t& waitinig, wait_t& stat, ou32_t cpu)
{
    plat_t::set_current_thread_name("writer");
    if (cpu)
    {
        plat_t::set_current_thread_cpu(*cpu);
    }

    while(waitinig.load(std::memory_order_consume) > 0);

    stat.waitCounter = writer_method_impl(total_events, w, c);

    c.writer_done();
}
