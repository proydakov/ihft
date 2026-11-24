#pragma once

#include <bitset>
#include <cstddef>

namespace ihft::impl
{

class cmdline
{
public:
    static constexpr std::size_t MAX_CPUS = 512;

    explicit cmdline(const char* file);

    static constexpr bool is_valid_cpu(unsigned cpu) noexcept
    {
        return cpu < MAX_CPUS;
    }

    bool is_isolated(unsigned cpu) const noexcept
    {
        return is_valid_cpu(cpu) && m_isolated.test(cpu);
    }

    bool is_nohz_fulled(unsigned cpu) const noexcept
    {
        return is_valid_cpu(cpu) && m_nohz_fulled.test(cpu);
    }

    bool is_rcu_nocbsed(unsigned cpu) const noexcept
    {
        return is_valid_cpu(cpu) && m_rcu_nocbsed.test(cpu);
    }

private:
    std::bitset<MAX_CPUS> m_isolated;
    std::bitset<MAX_CPUS> m_nohz_fulled;
    std::bitset<MAX_CPUS> m_rcu_nocbsed;
};

}
