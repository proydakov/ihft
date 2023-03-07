#include <channel/channel_factory.h>
#include <channel/one2one_seqnum_stream_object_queue.h>

#include <atomic>
#include <chrono>
#include <vector>
#include <thread>
#include <cstdlib>
#include <iostream>

using namespace ihft::channel;

int main()
{
    using spsc_queue_t = one2one_seqnum_stream_object_queue<std::string>;

    constexpr size_t queue_size = 1000; // will be rounded up to 1024
    constexpr size_t readers_count = 1; // spsc queue

    auto opt = channel_factory::make<spsc_queue_t>(queue_size, readers_count);
    if (not opt)
    {
        std::cerr << "[main thread] can't create a queue." << std::endl;
        return EXIT_FAILURE;
    }

    auto& p = opt->producer;
    auto& c = opt->consumers.front();

    std::cout << "[main thread] spawn threads" << std::endl;

    std::atomic_bool done{false};
    std::vector<std::thread> threads;
    threads.emplace_back([p = std::move(p), &done]() mutable
    {
        std::string buffer;
        std::cout << "[writer thread] type string value or <empty_line> to exit..." << std::endl;
        while(std::getline(std::cin, buffer) and not buffer.empty())
        {
            p.try_write(std::move(buffer));
        }
        done.store(true);
    });

    threads.emplace_back([c = std::move(c), &done]() mutable
    {
        while(not done.load(std::memory_order_relaxed))
        {
            auto opt = c.try_read();
            if (opt)
            {
                auto const& cref = static_cast<std::string>(*opt);
                std::cout << "[read thread] got: " << cref << std::endl;
            }
            else
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(250ms);
            }
        }
    });

    for(auto& t : threads)
    {
        t.join();
    }

    std::cout << "[main thread] done. See you next time)." << std::endl;

    return EXIT_SUCCESS;
}
