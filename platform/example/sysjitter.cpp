//
// platform_jitter measures the system induced jitter ("hiccups") a CPU bound thread experiences.
//
// This program was inspired by https://github.com/rigtorp/hiccups
//
// Linux kernel version 5.14 introduces the osnoise tracer that also measures the system jitter / noise.
// It additionally shows you the sources of the jitter.
// https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/trace/osnoise-tracer.rst
//

#include <compiler/compiler.h>
#include <platform/platform.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include <unistd.h>

using cclock_t = std::chrono::steady_clock;
using nanosec_t = std::chrono::nanoseconds;
using samples_t = std::vector<nanosec_t>;
using time_point_t = std::chrono::time_point<cclock_t>;

void help();

nanosec_t calculate_default_threshold();

int main(const int argc, char* argv[])
{
    std::ios::sync_with_stdio(false);

    auto runtime = std::chrono::seconds(5);
    auto threshold = nanosec_t::max();
    size_t nsamples = static_cast<size_t>(1 << 30) / sizeof(nanosec_t);

    bool invalid_options = false;

    int opt{};
    while ((opt = getopt(argc, argv, "r:t:s:h:")) != -1) {
        switch (opt) {
            case 'r':
                runtime = std::chrono::seconds(std::stoul(optarg));
                break;

            case 't':
                threshold = nanosec_t(std::stoul(optarg));
                break;

            case 's':
                nsamples = std::stoul(optarg);
                break;

            case 'h':
                help();
                return EXIT_SUCCESS;
                break;

            default:
                invalid_options = true;
                break;
        }
    }

    if (invalid_options)
    {
        help();
        return EXIT_FAILURE;
    }

    threshold = nanosec_t::max() == threshold ? calculate_default_threshold() : threshold;

    std::size_t const cores = std::thread::hardware_concurrency();

    std::vector<std::thread> threads;
    threads.reserve(cores);

    std::vector<samples_t> results(cores);
    for (auto& vec : results)
    {
        vec.reserve(nsamples);
    }

    std::vector<std::atomic<bool>> valid_experiment(cores);

    if (!ihft::platform::trait::lock_memory_pages(true, true))
    {
        std::cerr << "WARNING failed to lock memory, increase RLIMIT_MEMLOCK "
            "or run with CAP_IPC_LOC capability.\n";
    }

    std::atomic<size_t> active_threads = 0;

    auto functor = [](unsigned cpu, size_t cores, std::atomic<size_t>& active, std::vector<samples_t>& results, nanosec_t threshold, time_point_t until)
    {
        auto& output = results[cpu];

        if (!ihft::platform::trait::set_current_thread_cpu(cpu))
        {
            std::cerr << "can't set thread cpu" << std::endl;
            return false;
        }

        std::string const tname("sysjitter_" + std::to_string(cpu));
        if (!ihft::platform::trait::set_current_thread_name(tname.c_str()))
        {
            std::cerr << "can't set thread name" << std::endl;
            return false;
        }

        active++;

        while (active.load(std::memory_order_relaxed) != cores);

        auto ts1 = cclock_t::now();
        while (ts1 < until)
        {
            auto const ts2 = cclock_t::now();
            auto const delta = ts2 - ts1;
            if (delta < threshold)
            {
                ts1 = ts2;
            }
            else
            {
                if (output.size() == output.capacity()) {
                    [](unsigned cpu) IHFT_COLD {
                        auto const str = "WARNING preallocated sample space exceeded for cpu: "
                            + std::to_string(cpu)
                            + ", increase threshold or number of samples.\n";
                        std::cerr << str;
                    }(cpu);
                }
                output.emplace_back(delta);
                ts1 = cclock_t::now();
            }
        }

        return true;
    };

    auto const until = cclock_t::now() + runtime;

    for (unsigned i = 1; i < cores; i++)
    {
        threads.emplace_back([&, cpu = i]()
        {
            valid_experiment[cpu] = functor(cpu, cores, active_threads, results, threshold, until);
        });
    }

    valid_experiment[0] = functor(0, cores, active_threads, results, threshold, until);

    for (auto& t : threads)
        t.join();

    auto const cpuof = std::setw(4);
    auto const offset = std::setw(12);
    auto const thof = std::setw(15);

    std::cout << cpuof << "cpu" << " "
        << thof << "threshold_ns" << " "
        << offset << "hiccups" << " "
        << offset << "pct99_ns" << " "
        << offset << "pct999_ns" << " "
        << offset << "max_ns\n";

    for (size_t cpu = 0; cpu < cores; cpu++) {
        auto& s = results[cpu];

        if (valid_experiment[cpu])
        {
            std::sort(s.begin(), s.end());
            std::cout << cpuof << cpu << " "
              << thof << threshold.count() << " "
              << offset << s.size() << " "
              << offset << s[static_cast<size_t>(static_cast<double>(s.size()) * 0.99)].count() << " "
              << offset << s[static_cast<size_t>(static_cast<double>(s.size()) * 0.999)].count() << " "
              << offset << (!s.empty() ? s.back().count() : 0);
        }
        else
        {
            std::cout << cpuof << cpu
                << offset << " invalid experiment, please increase threshold or samples";
        }

        std::cout << std::endl;
    }

    return 0;
}

void help()
{
    std::cout << "platform_get_sysjitter 1.0.0 © 2022 Evgeny Proydakov\n"
        "https://github.com/proydakov/ihft/tree/master/platform/example\n"
        "usage: platform_get_jitter [-r runtime_seconds] [-t threshold_nanoseconds] [-s number_of_samples]\n";

    std::cout.flush();
}

// By default the threshold is calculated as 8 times the smallest difference
nanosec_t calculate_default_threshold()
{
    auto threshold = nanosec_t::max();

    auto t1 = cclock_t::now();
    for (int i = 0; i < 10'000; i++)
    {
        auto const t2 = cclock_t::now();
        auto const delta = t2 - t1;
        if (delta < threshold)
        {
            threshold = delta;
        }
        t1 = t2;
    }

    threshold *= 8;

    return threshold;
}
