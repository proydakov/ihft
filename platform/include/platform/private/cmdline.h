#pragma once

#include <bitset>

namespace ihft::impl
{

class cmdline
{
public:
    explicit cmdline(const char* file);

    bool is_isolated(unsigned cpu) const noexcept
    {
        return m_isolated.test(cpu);
    }

    bool is_nohz_fulled(unsigned cpu) const noexcept
    {
        return m_nohz_fulled.test(cpu);
    }

    bool is_rcu_nocbsed(unsigned cpu) const noexcept
    {
        return m_rcu_nocbsed.test(cpu);
    }

private:
    static constexpr size_t MAX_CPUS = 512;

    std::bitset<MAX_CPUS> m_isolated;
    std::bitset<MAX_CPUS> m_nohz_fulled;
    std::bitset<MAX_CPUS> m_rcu_nocbsed;
};

}
