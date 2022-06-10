#include <engine/engine_main.h>
#include <engine/cpus_configuration.h>
#include <engine/task_storage.h>

#include <chrono>
#include <iostream>

static const char * const DEMO_CFG = R"(
$ cat engine.toml # example
[engine]
cpu.netio = 6
cpu.algo = 7
cpu.logger = 8
)";

using namespace ihft::engine;

namespace
{
    void register_items(cpus_configuration const& cfg, task_storage& storage, std::atomic_bool const& until)
    {
        for(auto const& [name, _] : cfg.get_name_2_cpu())
        {
            storage.add_task(name, [](){
                return true;
            });
        }

        auto res = std::chrono::nanoseconds::max();

        auto t1 = std::chrono::steady_clock::now();
        for(int i = 0; i < 1'000'000 and until; i++)
        {
            auto const t2 = std::chrono::steady_clock::now();
            auto const delta = t2 - t1;
            if(delta < res)
            {
                res = delta;
            }
        }

        std::cout << "cpu threshold_ns: " << res.count() << std::endl;
    }

    void invalid_config()
    {
	std::cerr << DEMO_CFG;
	std::cerr.flush();
    }
}

int main(int const argc, const char * const argv[])
{
    register_tasks_callback_t reg_callback = register_items;
    invalid_configuration_callback_t inv_callback = invalid_config;

    std::cout << "Hello, %Username%.\n";
    std::cout << "This demo loading on any isolated CPU from config.\n";
    std::cout << "Generates several threads from configuration file.\n";
    std::cout << "And then waits for the SIGINT signal from the user.\n";
    std::cout.flush();

    return engine_main(argc, argv, reg_callback, inv_callback);
}
