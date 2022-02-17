#include <catch2/catch.hpp>

#include <engine/private/logical_cpu.h>

namespace
{
    static unsigned g_cpu{};
    static std::string g_cpu_name{};

    struct test_platform
    {
        static bool set_current_thread_cpu(unsigned cpu)
        {
            g_cpu = cpu;

            return true;
        }

        static bool set_current_thread_name(const char * const name)
        {
            g_cpu_name = name;

            return true;
        }

        static bool reset_current_thread_cpu()
        {
            g_cpu = 0;

            return true;
        }
    };
}

TEST_CASE("logical_cpu")
{
    {
        using test_logical_cpu = ihft::engine::logical_cpu_impl<test_platform>;

        test_logical_cpu cpu(7, "omega");
        REQUIRE(cpu.get_id() == 7);
        REQUIRE(cpu.get_name() == "omega");

        REQUIRE(g_cpu == 0);
        REQUIRE(g_cpu_name == "");

        REQUIRE(cpu.bind());

        REQUIRE(g_cpu == 7);
        REQUIRE(g_cpu_name == "omega");
    }

    REQUIRE(g_cpu == 0);
}
