#pragma once

#include <string>
#include <string_view>

namespace ihft::engine
{

template<typename platform>
class logical_cpu_impl final
{
public:
    logical_cpu_impl(unsigned cpu_id, std::string cpu_name) noexcept
        : m_binded(false)
        , m_cpu_id(cpu_id)
        , m_cpu_name(std::move(cpu_name))
    {
    }

    ~logical_cpu_impl()
    {
        if (m_binded)
        {
            m_binded = !platform::reset_current_thread_cpu();
        }
    }

    logical_cpu_impl(logical_cpu_impl&& other) noexcept = delete;
    logical_cpu_impl& operator=(logical_cpu_impl&& other) noexcept = delete;

    logical_cpu_impl(logical_cpu_impl const&) noexcept = delete;
    logical_cpu_impl& operator=(logical_cpu_impl const&) noexcept = delete;

    unsigned get_id() const noexcept
    {
        return m_cpu_id;
    }

    std::string_view get_name() const noexcept
    {
        return m_cpu_name;
    }

    // bind logical cpu to system cpu, also change system thread name
    bool bind() noexcept
    {
        return m_binded = platform::set_current_thread_cpu(m_cpu_id)
            && platform::set_current_thread_name(m_cpu_name.c_str());
    }

private:
    bool m_binded;
    unsigned m_cpu_id;
    std::string m_cpu_name;
};

}
