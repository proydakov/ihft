#include <catch2/catch.hpp>

#include <engine/cpus_configuration.h>

#include <misc/config_helper.h>

#include <types/temp_file.h>

using namespace ihft::misc;
using namespace ihft::types;
using namespace ihft::engine;

namespace
{
    template<int CORES, bool ISOL, bool NOHZ, bool NOCBS>
    struct test_platform
    {
        static unsigned get_total_cpus() noexcept
        {
            return CORES;
        }

        static bool get_cpu_isolation_status(unsigned) noexcept
        {
            return ISOL;
        }

        static bool get_cpu_nohz_full_status(unsigned) noexcept
        {
            return NOHZ;
        }

        static bool get_cpu_rcu_nocbs_status(unsigned) noexcept
        {
            return NOCBS;
        }
    };

    using test_isolated_platform = test_platform<16, true, true, true>;
    using test_unisolated_platform = test_platform<16, false, true, true>;
    using test_unnohz_full_platform = test_platform<16, true, false, true>;
    using test_unrcu_nocbs_platform = test_platform<16, true, true, false>;

    bool contains(std::string_view str, std::string_view pattern)
    {
        return str.find(pattern) != std::string_view::npos;
    }
}

TEST_CASE("cpus_configuration : config_helper")
{
    const char* const VALID_TOML_DOC =
    R"([engine]
cpu.netio = 6
cpu.strategy = 7
cpu.logger = 13
#cpu.backup = 12
)";

    temp_file file("cpus_configuration_config_helper", VALID_TOML_DOC);
    auto const result = config_helper::parse(file.fpath());
    REQUIRE(result);

    auto const cpus_config = cpus_configuration::parse<test_isolated_platform>(result.value());
    REQUIRE(cpus_config);

    auto const& name2cpu = cpus_config.value();

    REQUIRE(name2cpu.get_name_2_cpu().size() == 3);
}

TEST_CASE("cpus_configuration : valid")
{
    auto const config = cpus_configuration::parse<test_isolated_platform>({
        {"netio", 6}, {"strategy", 7}, {"logger", 13}
    });

    REQUIRE(config);
}

TEST_CASE("cpus_configuration : invalid - empty")
{
    auto const config = cpus_configuration::parse<test_isolated_platform>({});

    REQUIRE(config.failed());

    REQUIRE(contains(config.error(), "Empty configuration"));
}

TEST_CASE("cpus_configuration : invalid - unreal cpu")
{
    auto const config = cpus_configuration::parse<test_isolated_platform>({
        {"alpha", 1024}
    });

    REQUIRE(config.failed());

    REQUIRE(contains(config.error(), "total_cpus"));
}

TEST_CASE("cpus_configuration : invalid - same core")
{
    auto const config = cpus_configuration::parse<test_isolated_platform>({
        {"alpha", 0},
        {"omega", 0}
    });

    REQUIRE(config.failed());

    REQUIRE(contains(config.error(), "[alpha, omega] use same core"));
}

TEST_CASE("cpus_configuration : invalid - unisolated")
{
    auto const config = cpus_configuration::parse<test_unisolated_platform>({
        {"alpha", 0},
        {"omega", 1}
    });

    REQUIRE(config.failed());

    REQUIRE(contains(config.error(), "isolation"));
}

TEST_CASE("cpus_configuration : invalid - nohz_full")
{
    auto const config = cpus_configuration::parse<test_unnohz_full_platform>({
        {"alpha", 0},
        {"omega", 1}
    });

    REQUIRE(config.failed());

    REQUIRE(contains(config.error(), "nohz_full"));
}

TEST_CASE("cpus_configuration : invalid - rcu_nocbs")
{
    auto const config = cpus_configuration::parse<test_unrcu_nocbs_platform>({
        {"alpha", 0},
        {"omega", 1}
    });

    REQUIRE(config.failed());

    REQUIRE(contains(config.error(), "rcu_nocbs"));
}
