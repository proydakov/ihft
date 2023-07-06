#include <logger/logger.h>
#include <logger/logger_adapter.h>
#include <platform/platform.h>

#include <string>
#include <thread>
#include <vector>
#include <iostream>

/*

UTC 2023-06-22 20:56:09.668061  INFO [thread_5:1581221] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 5, thread_id: 1581221, tname: thread_5, event id: 1021
UTC 2023-06-22 20:56:09.668119  INFO [thread_0:1581216] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 0, thread_id: 1581216, tname: thread_0, event id: 1020
UTC 2023-06-22 20:56:09.664770  INFO [thread_1:1581217] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 1, thread_id: 1581217, tname: thread_1, event id: 1024
UTC 2023-06-22 20:56:09.668944  INFO [thread_2:1581218] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 2, thread_id: 1581218, tname: thread_2, event id: 1022
UTC 2023-06-22 20:56:09.669717  INFO [thread_6:1581222] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 6, thread_id: 1581222, tname: thread_6, event id: 1022
UTC 2023-06-22 20:56:09.669239  INFO [thread_3:1581219] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 3, thread_id: 1581219, tname: thread_3, event id: 1022
UTC 2023-06-22 20:56:09.667791  INFO [thread_4:1581220] logger_mthreads.cpp(56):'auto main()::(anonymous class)::operator()() const' from id: 4, thread_id: 1581220, tname: thread_4, event id: 1022

*/

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
            logger_adapter::set_thread_name(tname.c_str());
            trait::set_current_thread_name(tname.c_str());
            trait::set_current_thread_cpu(i);

            auto const tid = trait::get_thread_id();

            for(size_t j = 0; j < ITERATIONS; j++)
            {
                IHFT_LOG_INFO(
                    "from id: {}, thread_id: {}, tname: {}, event id: {}",
                    i, tid, tname, j
                );
            }

            alive--;
        });
    }

    while(alive.load(std::memory_order_relaxed) > 0)
    {
        logger_adapter::dispatch();
    }

    for(auto& t : threads)
    {
        t.join();
    }

    while(logger_adapter::dispatch());

    return 0;
}
