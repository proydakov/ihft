#include <catch2/catch.hpp>

#include <platform/private/cmdline.h>
#include <types/temp_file.h>

#include <string>
#include <vector>
#include <iostream>

using namespace ihft::types;

namespace
{

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec)
{
    os << "[ ";
    for(auto const& elem : vec)
    {
        os << elem << ' ';
    }
    os << "]";
    return os;
}

}

void test_impl(temp_file const& file, std::vector<unsigned> result)
{
    std::vector<unsigned> data_isolated;
    std::vector<unsigned> data_nohz_fulled;
    std::vector<unsigned> data_rcu_nocbsed;

    ihft::impl::cmdline cmdline(file.fpath().c_str());

    for(unsigned cpu = 0; cpu < 64; cpu++)
    {
        if(cmdline.is_isolated(cpu))
        {
            data_isolated.push_back(cpu);
        }

        if(cmdline.is_nohz_fulled(cpu))
        {
            data_nohz_fulled.push_back(cpu);
        }

        if(cmdline.is_rcu_nocbsed(cpu))
        {
            data_rcu_nocbsed.push_back(cpu);
        }
    }

    REQUIRE( data_isolated == result );
    REQUIRE( data_nohz_fulled == result );
    REQUIRE( data_rcu_nocbsed == result );
}

TEST_CASE("test_1")
{
    temp_file file("cmdline_1.txt",
        "BOOT_IMAGE=/boot/vmlinuz-3.16.0-44-generic auto noprompt priority=critical locale=en_US isolcpus=3,5,7 nohz_full=3,5,7 rcu_nocbs=3,5,7 quiet\r\n");

    test_impl(file, {3,5,7});
}

TEST_CASE("test_2")
{
    temp_file file("cmdline_2.txt",
        "BOOT_IMAGE=/boot/vmlinuz-3.16.0-44-generic auto noprompt priority=critical locale=en_US quiet nohz_full=9,17,2 rcu_nocbs=9,17,2 isolcpus=9,17,2\r\n");

    test_impl(file, {2,9,17});
}

TEST_CASE("test_3")
{
    temp_file file("cmdline_3.txt",
        "BOOT_IMAGE=/boot/vmlinuz-3.16.0-44-generic auto noprompt priority=critical quiet isolcpus=8-11,17 nohz_full=8-11,17 rcu_nocbs=8-11,17 locale=en_US\r\n");

    test_impl(file, {8,9,10,11,17});
}
