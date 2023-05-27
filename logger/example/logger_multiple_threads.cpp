#include <logger/logger_event.h>
#include <logger/logger_client.h>
#include <logger/logger_adapter.h>

#include <platform/platform.h>

#include <string>
#include <thread>
#include <vector>
#include <iostream>

#include <pthread.h>
#include <x86intrin.h>

using namespace ihft::logger;
using namespace ihft::platform;

constexpr size_t ITERATIONS = 1 * 1024 * 1024;

int main()
{
    logger_adapter::change_mode(logger_adapter::mode_t::async);

    unsigned const cpus = std::max(1u, trait::get_total_cpus() - 1);
    trait::set_current_thread_cpu(cpus);

    std::atomic<std::uint64_t> alive{};

    std::vector<std::thread> threads;
    for(unsigned i = 0; i < cpus; i++)
    {
        alive++;

        threads.emplace_back([i, &alive](){
            logger_adapter::logger_client_thread_guard guard;

            std::string const tname("thread_" + std::to_string(i));
            trait::set_current_thread_name(tname.c_str());
            trait::set_current_thread_cpu(i);

            auto const tid = trait::get_thread_id();
            auto client = logger_client::get_this_thread_client();

            for(size_t j = 0; j < ITERATIONS; j++)
            {
                auto event_slab = client->active_event_slab();

                auto event = std::construct_at(event_slab,
                    "from id: {}, thread_id: {}, thread_name: {}, event id: {}",
                    i, tid, tname, j
                );

                while(not client->try_log_event(event))
                {
                    _mm_pause();
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
