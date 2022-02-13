#include <engine/logical_cpu.h>

#include <platform/platform.h>

namespace ihft::engine
{

logical_cpu::logical_cpu(unsigned cpu_id, std::string cpu_name) noexcept
    : m_binded(false)
    , m_cpu_id(cpu_id)
    , m_cpu_name(std::move(cpu_name))
{
}

logical_cpu::~logical_cpu()
{
    if (m_binded)
    {
        m_binded = !ihft::platform::reset_current_thread_cpu();
    }
}

bool logical_cpu::bind() noexcept
{
    return m_binded = ihft::platform::set_current_thread_cpu(m_cpu_id)
        && ihft::platform::set_current_thread_name(m_cpu_name.c_str());
}

}
