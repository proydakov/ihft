#pragma once

#include <types/function_ref.h>

#include <atomic>

namespace ihft::engine
{
    class task_storage;
    class cpus_configuration;

    using register_tasks_callback_t = types::function_ref<void(cpus_configuration const&, task_storage&, std::atomic_bool const&)>;
    using invalid_configuration_callback_t = types::function_ref<void()>;

    int engine_main(int const argc, char const * const argv[], register_tasks_callback_t, invalid_configuration_callback_t);
}
