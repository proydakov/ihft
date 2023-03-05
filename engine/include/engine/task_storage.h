#pragma once

#include <map>
#include <string>
#include <functional>

namespace ihft::engine
{

namespace impl
{
    class engine;
}

class task_storage
{
public:
    using task_t = std::function<bool()>;

    void add_task(std::string name, task_t task)
    {
        m_tasks.emplace(std::move(name), std::move(task));
    }

    auto const& get_tasks() const noexcept
    {
        return m_tasks;
    }

private:
    friend class impl::engine;

    std::multimap<std::string, task_t> m_tasks;
};

}
