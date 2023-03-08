#pragma once

#include <types/function_ref.h>

#include <atomic>

namespace ihft::engine
{
    class cpus_config;
    class task_storage;

    using register_tasks_callback_t = types::function_ref<bool(cpus_config const&, task_storage&, std::atomic_bool const&)>;
    using invalid_config_callback_t = types::function_ref<void()>;

    int engine_main(int const argc, char const * const argv[], register_tasks_callback_t, invalid_config_callback_t);
}
