#include <logger/logger_event.h>
#include <logger/logger_client.h>
#include <logger/logger_adapter.h>

#include <platform/platform.h>

#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

namespace ihft::logger
{
DECLARE_SIMPLE_LOGGER_CONTRACT(std::thread::id);
}

using namespace ihft::logger;

constexpr size_t ITERATIONS = 1 * 1024 * 1024;

int main()
{
    logger_adapter::change_mode(logger_adapter::mode_t::async);

    size_t const cpus = std::thread::hardware_concurrency();

    std::atomic<std::uint64_t> alive{};

    std::vector<std::thread> threads;
    for(size_t i = 0; i < cpus; i++)
    {
        alive++;

        threads.emplace_back([i, &alive](){
            logger_adapter::logger_client_thread_guard guard;

            std::string const tname("thread_" + std::to_string(i));
            ihft::platform::trait::set_current_thread_name(tname.c_str());

            auto client = logger_client::get_this_thread_client();

            for(size_t j = 0; j < ITERATIONS; j++)
            {
                auto event_slab = client->active_event_slab();

                auto event = std::construct_at(event_slab,
                    "from id: {}, thread_id: {}, thread_name: {}, event id: {}",
                    i, std::this_thread::get_id(), tname, j
                );

                while(not client->try_log_event(event))
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(25));
                }
            }

            alive--;
        });
    }

    std::uint64_t total{};

    while(alive.load(std::memory_order_relaxed) > 0)
    {
        if(logger_adapter::dispatch())
        {
            total++;
        }
    }

    for(auto& t : threads)
    {
        t.join();
    }

    while(logger_adapter::dispatch())
    {
        total++;
    }

    std::cout << "total: " << total << std::endl;

    return 0;
}
