#pragma once

#include <bitset>

namespace ihft::impl
{

class isolation
{
public:
    explicit isolation(const char* file);
    bool is_isolated(unsigned cpu) const
    {
        return m_isolated.test(cpu);
    }

private:
    std::bitset<512> m_isolated;
};

}
