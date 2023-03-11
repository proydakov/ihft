#include <engine/engine_main.h>
#include <engine/cpus_config.h>
#include <engine/task_storage.h>
#include <platform/platform.h>

#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace ihft::engine;
using namespace ihft::platform;

namespace
{
    bool register_items(cpus_config const& cfg, task_storage& storage, std::atomic_bool const&)
    {
        for(auto const& [name, _] : cfg.get_name_2_cpu())
        {
            storage.add_task(name, [](){
                return true;
            });
        }

        return true;
    }

    void invalid_config()
    {
    }

    bool generate_valid_config()
    {
        std::vector<std::string> frames = 
        {
            "Зодиак",
            "Роутер",
            "Арк-9",
            "Пионер",
            "Cтранник",
            "Орган",
            "Банч",
            "Zкзистор",
            "Кластер"
        };

        std::random_device rd;
        std::mt19937 rg(rd());
        std::shuffle(frames.begin(), frames.end(), rg);

        std::ofstream output("engine.toml");
        output << "[engine]\n";

        auto it = frames.begin();

        unsigned const cpus = std::thread::hardware_concurrency();
        for(unsigned c = 0; c < cpus and it != frames.end(); c++)
        {
            if (trait::get_cpu_isolation_status(c))
            {
                output << "cpu." << '"' << *it << '"' << " = " << c << '\n';
                it++;
            }
        }

        if (it == frames.begin())
        {
            std::cerr << "I can't find suitable isolated cores to generate the correct configuration.\n";
            std::cerr.flush();
            return false;
        }
        else
        {
            output << "\n";
            return true;
        }
    }
}

int main(int const, const char * const argv[])
{
    std::cout << "Hello, %Username%.\n";
    std::cout << "This demo loading on any isolated CPU from config.\n";
    std::cout << "Generates several threads from configuration file.\n";
    std::cout << "And then waits for the SIGINT signal from the user.\n";
    std::cout << "\n";
    std::cout << "The thread names were inspired by:\n";
    std::cout << "https://github.com/KD-lab-Open-Source/Perimeter\n";
    std::cout << "https://store.steampowered.com/app/289440/Perimeter\n";

    std::cout.flush();

    if (not generate_valid_config())
    {
        return EXIT_FAILURE;
    }

    std::vector<const char *> params;
    params.push_back(argv[0]);
    params.push_back("engine.toml");

    return engine_main(static_cast<int>(params.size()), params.data(), register_items, invalid_config);
}
