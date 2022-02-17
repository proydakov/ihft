#pragma once

#include <types/function_ref.h>

#include <atomic>

namespace ihft::engine
{
    class task_storage;
    class cpus_configuration;

    using reg_tasks_callback_t = types::function_ref<void(cpus_configuration const&, task_storage&, std::atomic_bool const&)>;

    int engine_main(int const argc, char const * const argv[], reg_tasks_callback_t register_tasks);
}
