#pragma once

#include <string>
#include <string_view>

namespace ihft::engine
{

class logical_cpu final
{
public:
    logical_cpu(unsigned cpu_id, std::string cpu_name) noexcept;

    ~logical_cpu();

    logical_cpu(logical_cpu&& other) noexcept = delete;
    logical_cpu& operator=(logical_cpu&& other) noexcept = delete;

    logical_cpu(logical_cpu const&) noexcept = delete;
    logical_cpu& operator=(logical_cpu const&) noexcept = delete;

    unsigned get_id() const noexcept
    {
        return m_cpu_id;
    }

    std::string_view get_name() const noexcept
    {
        return m_cpu_name;
    }

    // bind logical cpu to system cpu, also change system thread name
    bool bind() noexcept;

private:
    bool m_binded;
    unsigned m_cpu_id;
    std::string m_cpu_name;
};

}
