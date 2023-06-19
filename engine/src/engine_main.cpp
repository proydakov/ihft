#include <engine/engine_main.h>
#include <engine/private/engine.h>
#include <engine/private/logical_cpu.h>
#include <engine/cpus_config.h>
#include <engine/task_storage.h>

#include <platform/platform.h>
#include <misc/config_helper.h>
#include <misc/signal_helper.h>

#include <logger/logger_adapter.h>

#include <cstdlib>
#include <iostream>
#include <filesystem>

using plf = ihft::platform::trait;
using logadap = ihft::logger::logger_adapter;

namespace
{
    const char * const LOGGER_TASK = "logger";

    std::atomic_bool g_until{true};

    void mysa_sigaction(int signal, siginfo_t*, void*)
    {
        g_until = false;
        printf("Catch signal: %s. Stopping...\n", strsignal(signal));
    }

    int check_system()
    {
        if (not plf::is_scaling_governor_use_performance_mode())
        {
            std::cerr << "Is CPU scaling governor doesn't use performance mode." << std::endl;
            std::cerr << "Exec from root: find /sys/devices/system/cpu -name scaling_governor -exec sh -c 'echo performan" << std::endl;
            return EXIT_FAILURE;
        }

        if (plf::is_hyper_threading_active())
        {
            std::cerr << "Hyper threading is active. Disable it in BIOS" << std::endl;
            return EXIT_FAILURE;
        }

        if (plf::is_swap_active())
        {
            std::cerr << "Swap is active. Disable it: sudo swapoff -a" << std::endl;
            return EXIT_FAILURE;
        }

        if (plf::is_transparent_hugepages_active())
        {
            std::cerr << "Transparent hugepages is active." << std::endl;
            std::cerr << "Exec from root: echo never > /sys/kernel/mm/transparent_hugepage/enabled" << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}

namespace ihft::engine
{
    namespace impl
    {
        class engine_main_helper final
        {
        public:
            static void replace_task(task_storage& storage, std::string name, task_storage::task_t task)
            {
                storage.replace_task(std::move(name), std::move(task));
            }
        };
    }

    int engine_main(int const argc, char const * const argv[], register_tasks_callback_t register_tasks, invalid_config_callback_t invalid_cfg)
    {
        // we are going to use a pure C++ application
        std::ios::sync_with_stdio(false);

        std::cout << std::endl;

        if (check_system() == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }

        auto const usage = [argv, invalid_cfg]()
        {
            std::cerr << "Usage: " << argv[0] << " <path/to/config>" << std::endl;
            invalid_cfg();
        };

        if (argc < 2)
        {
            usage();
            return EXIT_FAILURE;
        }

        std::string_view const path_to_config = argv[1];
        if (not std::filesystem::exists(path_to_config) || !std::filesystem::is_regular_file(path_to_config))
        {
            usage();
            return EXIT_FAILURE;
        }

        std::cout << "parsing config file..." << std::endl;

        auto const helper_res = misc::config_helper::parse(path_to_config);
        if (not helper_res)
        {
            usage();
            std::cerr << helper_res.error() << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "parsing cpu configuration..." << std::endl;

        auto const& helper = helper_res.value();
        auto const cpu_cfg_res = cpus_config::parse<platform::trait>(helper);
        if (not cpu_cfg_res)
        {
            usage();
            std::cerr << cpu_cfg_res.error() << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "setuping signal handlers..." << std::endl;

        if (not misc::setup_sigaction_handler(&mysa_sigaction, {SIGINT}))
        {
            std::cerr << "can't setup signal handler for SIGINT" << std::endl;
            return EXIT_FAILURE;
        }

        if (not misc::block_application_signals({SIGUSR1, SIGUSR2}))
        {
            std::cerr << "can't block signals: SIGUSR1, SIGUSR2" << std::endl;
        }

        auto const& cpu_cfg = cpu_cfg_res.value();

        task_storage storage;

        std::cout << "registering tasks..." << std::endl;

        {
            if (cpu_cfg.get_name_2_cpu().empty())
            {
                std::cerr << "cpu configuration can't be empty: " << path_to_config << std::endl;
                return EXIT_FAILURE;
            }

            // the correct cpus configuration can't be empty
            // we are going to move initialization into the first isolated cpu
            unsigned const cpu_id = cpu_cfg.get_name_2_cpu().begin()->second;
            std::string const tname_main("main");

            impl::logical_cpu_impl<plf> logical_cpu(cpu_id, tname_main);
            if (not logical_cpu.bind())
            {
                std::cerr << "runtime error: can't bind thread into core" << std::endl;
                return EXIT_FAILURE;
            }

            logger::logger_adapter::set_thread_name(tname_main.c_str());

            if (not register_tasks(cpu_cfg, storage, g_until))
            {
                std::cerr << "can't register tasks" << std::endl;
                return EXIT_FAILURE;
            }
        }

        if (not g_until)
        {
            return EXIT_SUCCESS;
        }

        if (cpu_cfg.get_name_2_cpu().contains(LOGGER_TASK))
        {
            impl::engine_main_helper::replace_task(storage, LOGGER_TASK, []()
            {
                logadap::dispatch();
                return true;
            });
        }

        std::cout << "creating engine..." << std::endl;

        auto engine_res = impl::engine::create(cpu_cfg, std::move(storage), g_until);
        if (not engine_res)
        {
            usage();
            std::cerr << engine_res.error() << std::endl;
            return EXIT_FAILURE;
        }

        if (cpu_cfg.get_name_2_cpu().contains(LOGGER_TASK))
        {
            logadap::change_mode(logadap::mode_t::async);
        }

        std::cout << "engine joining..." << std::endl;

        auto& engine = engine_res.value();
        engine.join();

        return EXIT_SUCCESS;
    }
}
