#include <engine/private/engine.h>
#include <engine/private/logical_cpu.h>

#include <logger/logger_adapter.h>

#include <platform/platform.h>

#include <set>

namespace ihft::engine::impl
{
    engine::engine_result_t engine::create(cpus_config cfg, task_storage storage, std::atomic_bool const& until)
    {
        std::string const error("Invalid configuration. ");

        auto const cpus = cfg.get_name_2_cpu().size();
        auto const tasks = storage.get_tasks().size();
        if (cpus != tasks)
        {
            return error + "cpus: [" + std::to_string(cpus) + "] != tasks[" + std::to_string(tasks) + "]";
        }

        std::set<std::string> task;

        for (auto const& [name, _] : storage.get_tasks())
        {
            auto const [_it, res] = task.insert(name);
            if (!res)
            {
                return error + "duplicated tasks: " + name;
            }

            if (cfg.get_name_2_cpu().find(name) == cfg.get_name_2_cpu().end())
            {
                return error + "task: [" + name + "] doesn't have logical cpu.";
            }
        }
        return engine(std::move(cfg), std::move(storage), until);
    }

    engine::engine(cpus_config cfg, task_storage storage, std::atomic_bool const& until)
        : m_joined(false)
    {
        for (auto const& [name, cpu] : cfg.get_name_2_cpu())
        {
            auto it = storage.m_tasks.find(name);
            if (it == storage.m_tasks.end())
            {
                ::abort();
            }

            m_threads.emplace_back([&until, task = std::move(it->second), name = name, cpu = cpu]()
            {
                logical_cpu_impl<ihft::platform::trait> lcpu(cpu, name);
                logger::logger_adapter::logger_client_thread_guard guard;

                logger::logger_adapter::set_thread_name(name.c_str());

                if (not lcpu.bind())
                {
                    return;
                }

                while (until.load(std::memory_order_relaxed) and task());
            });
        }
    }

    engine::~engine()
    {
        join();
    }

    void engine::join()
    {
        if (!m_joined)
        {
            for (auto& t : m_threads)
            {
                t.join();
            }

            m_joined = true;
        }
    }
}
