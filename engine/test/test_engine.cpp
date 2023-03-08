#include <catch2/catch.hpp>

#include <engine/private/engine.h>
#include <engine/task_storage.h>
#include <engine/cpus_config.h>

using namespace ihft::engine;
using namespace ihft::engine::impl;

namespace
{
    std::atomic_bool g_until{};

    struct test_platform
    {
        static unsigned get_total_cpus() noexcept
        {
            return 16;
        }

        static bool get_cpu_isolation_status(unsigned) noexcept
        {
            return true;
        }

        static bool get_cpu_nohz_full_status(unsigned) noexcept
        {
            return true;
        }

        static bool get_cpu_rcu_nocbs_status(unsigned) noexcept
        {
            return true;
        }
    };
}

TEST_CASE("engine - not enough CPU")
{
    auto const cpus_res = cpus_config::parse<test_platform>({{"alpha", 7}, {"omega", 9}});
    REQUIRE(cpus_res);
    auto const& cpus = cpus_res.value();

    auto func = std::function<bool()>([](){
        return false;
    });

    task_storage storage;
    storage.add_task("alpha", func);
    storage.add_task("omega", func);
    storage.add_task("omicron", func);

    auto const res = engine::create(cpus, std::move(storage), g_until);
    REQUIRE(!res);

    REQUIRE(res.error() == "Invalid configuration. cpus: [2] != tasks[3]");
}

TEST_CASE("engine - duplicated tasks")
{
    auto const cpus_res = cpus_config::parse<test_platform>({{"alpha", 7}, {"omega", 9}});
    REQUIRE(cpus_res);
    auto const& cpus = cpus_res.value();

    auto func = std::function<bool()>([](){
        return false;
    });

    task_storage storage;
    storage.add_task("alpha", func);
    storage.add_task("alpha", func);

    auto const res = engine::create(cpus, std::move(storage), g_until);
    REQUIRE(!res);

    REQUIRE(res.error() == "Invalid configuration. duplicated tasks: alpha");
}

TEST_CASE("engine - invalid assignment")
{
    auto const cpus_res = cpus_config::parse<test_platform>({{"alpha", 7}, {"omega", 9}});
    REQUIRE(cpus_res);
    auto const& cpus = cpus_res.value();

    auto func = std::function<bool()>([](){
        return false;
    });

    task_storage storage;
    storage.add_task("alpha", func);
    storage.add_task("omicron", func);

    auto const res = engine::create(cpus, std::move(storage), g_until);
    REQUIRE(!res);

    REQUIRE(res.error() == "Invalid configuration. task: [omicron] doesn't have logical cpu.");
}
