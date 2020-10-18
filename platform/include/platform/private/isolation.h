#pragma once

#include <vector>

namespace ihft::impl
{

class isolation
{
public:
    explicit isolation(const char* file);

    bool is_isolated(unsigned long cpu) const;

private:
    std::vector<unsigned long> m_isolated;
};

}
