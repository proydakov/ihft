#pragma once

#include <map>
#include <string>
#include <functional>

namespace ihft::engine
{

namespace impl
{
    class engine;
    class engine_main_helper;
}

class task_storage final
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
    void replace_task(std::string name, task_t task)
    {
        std::erase_if(m_tasks, [&name](const auto& item) {
            return item.first == name;
        });

        m_tasks.emplace(std::move(name), std::move(task));
    }

private:
    friend class impl::engine;
    friend class impl::engine_main_helper;

    std::multimap<std::string, task_t> m_tasks;
};

}
