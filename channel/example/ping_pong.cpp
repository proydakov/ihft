#include <constant/constant.h>
#include <channel/channel_factory.h>
#include <channel/one2one_seqnum_stream_pod_queue.h>

#include <vector>
#include <thread>
#include <cstdlib>
#include <iostream>

using namespace ihft::channel;

struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) result_t
{
    std::chrono::nanoseconds min_rtt = std::chrono::nanoseconds::max();
    std::chrono::nanoseconds max_rtt = std::chrono::nanoseconds::min();
};

int main()
{
    using spsc_queue_t = one2one_seqnum_stream_pod_queue<size_t>;

    constexpr size_t queue_size = 1000; // will be rounded up to 1024
    constexpr size_t readers_count = 1; // spsc queue

    auto opt_ab = channel_factory::make<spsc_queue_t>(queue_size, readers_count);
    auto opt_ba = channel_factory::make<spsc_queue_t>(queue_size, readers_count);

    if (not opt_ab or not opt_ba)
    {
        std::cerr << "[main thread] can't create a queue." << std::endl;
        return EXIT_FAILURE;
    }

    auto& p_ab = opt_ab->producer;
    auto& c_ab = opt_ab->consumers.front();

    auto& p_ba = opt_ba->producer;
    auto& c_ba = opt_ba->consumers.front();

    std::cout << "[main thread] spawn threads" << std::endl;

    constexpr size_t repeats = 1024;
    constexpr size_t iters = 256;

    result_t result;

    std::vector<std::thread> threads;

    threads.emplace_back([p_ab = std::move(p_ab), c_ba = std::move(c_ba), &result, &iters]() mutable
    {
        for(size_t j = 0; j < repeats; j++)
        {
            auto const ts1 = std::chrono::steady_clock::now();

            for(size_t i = 0; i < iters; i++)
            {
                while(not p_ab.try_write(size_t{i}));

                std::optional<size_t> opt;
                do
                {
                    opt = c_ba.try_read();
                }
                while(not opt);

                if (*opt != i) [[unlikely]]
                {
                    ::abort();
                }
            }

            auto const ts2 = std::chrono::steady_clock::now();

            std::chrono::nanoseconds const real_rtt = (ts2 - ts1) / iters;
            result.min_rtt = std::min(result.min_rtt, real_rtt);
            result.max_rtt = std::max(result.max_rtt, real_rtt);
        }
    });

    threads.emplace_back([c_ab = std::move(c_ab), p_ba = std::move(p_ba)]() mutable
    {
        for(size_t j = 0; j < repeats; j++)
        {
            for(size_t i = 0; i < iters; i++)
            {
                std::optional<size_t> opt;
                do
                {
                    opt = c_ab.try_read();
                }
                while(not opt);

                while(not p_ba.try_write(size_t{*opt}));
            }
        }
    });

    for(auto& t : threads)
    {
        t.join();
    }

    std::cout << "[main thread] T(a) -> T(b) -> T(a) latency report" << std::endl;
    std::cout << "[main thread] min rtt: " << result.min_rtt.count() << " nanoseconds" << std::endl;
    std::cout << "[main thread] max rtt: " << result.max_rtt.count() << " nanoseconds" << std::endl;
    std::cout << "[main thread] done" << std::endl;

    return EXIT_SUCCESS;
}
