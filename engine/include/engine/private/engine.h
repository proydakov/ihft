#pragma once

#include <engine/task_storage.h>
#include <engine/cpus_configuration.h>

#include <types/result.h>

#include <atomic>
#include <thread>
#include <vector>

namespace ihft::engine
{

class engine
{
public:
    using engine_result_t = ihft::types::result<engine, std::string>;

    static engine_result_t create(cpus_configuration, task_storage, std::atomic_bool const&);
    ~engine();

    engine(engine const&) = delete;
    engine(engine&&) noexcept = default;

    engine& operator=(engine const&) = delete;
    engine& operator=(engine&&) noexcept = delete;

    void join();

private:
    engine(cpus_configuration, task_storage, std::atomic_bool const&);

private:
    std::vector<std::thread> m_threads;
    bool m_joined;
};

}
