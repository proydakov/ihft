//
// This program was inspired by https://github.com/rigtorp/c2clat
//
// A tool to measure CPU core to core latency (inter-core latency).
//

#include <constant/constant.h>
#include <platform/platform.h>

#include <map>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include <numeric>
#include <iomanip>
#include <iostream>

using plf = ihft::platform::trait;

int main()
{
    auto const nthreads = std::thread::hardware_concurrency();

    std::vector<unsigned> cpus(nthreads);
    std::iota(cpus.begin(), cpus.end(), 0);

    std::map<std::pair<unsigned, unsigned>, std::chrono::nanoseconds> results;

    constexpr int repeats = 1024;
    constexpr int iters = 256;

    for(unsigned i = 0; i < cpus.size(); i++)
    {
        for(unsigned j = i + 1; j < cpus.size(); j++)
        {
            alignas(ihft::constant::CPU_CACHE_LINE_SIZE) std::atomic<int> seq1 = {-1};
            alignas(ihft::constant::CPU_CACHE_LINE_SIZE) std::atomic<int> seq2 = {-1};

            auto t = std::thread([&](){
                plf::set_current_thread_cpu(cpus[i]);
                for (int k = 0; k < repeats; k++)
                {
                    for(int h = 0; h < iters; h++)
                    {
                        while(seq1.load(std::memory_order_acquire) != h);
                        seq2.store(h, std::memory_order_release);
                    }
                }
            });

            std::chrono::nanoseconds rtt = std::chrono::nanoseconds::max();

            plf::set_current_thread_cpu(cpus[j]);
            for (int k = 0; k < repeats; k++)
            {
                seq1 = seq2 = -1;
                auto const ts1 = std::chrono::steady_clock::now();
                for (int h = 0; h < iters; h++)
                {
                    seq1.store(h, std::memory_order_release);
                    while(seq2.load(std::memory_order_acquire) != h);
                }
                auto const ts2 = std::chrono::steady_clock::now();
                rtt = std::min(rtt, ts2 - ts1);
            }

            t.join();

            auto const real_rtt = rtt / 2 / iters;
            results[{i, j}] = real_rtt;
            results[{j, i}] = real_rtt;
        }
    }

    std::cout << "Core <-> Core latency report. Units: (nanoseconds)\n";
    std::cout << std::setw(4) << "CPU";
    for (size_t i = 0; i < cpus.size(); ++i)
    {
        std::cout << " " << std::setw(4) << cpus[i];
    }
    std::cout << "\n";
    for (size_t i = 0; i < cpus.size(); ++i)
    {
        std::cout << std::setw(4) << cpus[i];
        for (size_t j = 0; j < cpus.size(); ++j)
        {
            std::cout << " " << std::setw(4) << results[{i, j}].count();
        }
        std::cout << "\n";
    }
    std::cout.flush();

    return 0;
}
