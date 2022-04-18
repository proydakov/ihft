#include <misc/config_helper.h>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.h>

#include <memory>
#include <sstream>
#include <ostream>

namespace ihft::misc
{
    struct config_helper::impl
    {
        impl(toml::table t) : table(std::move(t)) {}

        toml::table table;
    };

    template<typename T>
    config_helper::config_result config_helper::parse_impl(T file_path)
    {
        auto config = toml::parse_file(file_path);

        if (config)
        {
            return config_helper(std::move(config.table()));
        }
        else
        {
            std::stringstream sstream;
            sstream << config.error();
            return sstream.str();
        }
    }

    config_helper::config_result config_helper::parse(std::string_view file_path)
    {
        return parse_impl(file_path);
    }

    template<typename T>
    config_helper::config_helper(T table) noexcept
        : m_impl(std::make_unique<impl>(std::move(table)))
    {
    }

    config_helper::~config_helper()
    {
    }

    config_helper::config_helper(config_helper&& other) noexcept
    {
        m_impl = std::move(other.m_impl);
    }

    config_helper& config_helper::operator=(config_helper&& other) noexcept
    {
        if (&other != this)
        {
            m_impl = std::move(other.m_impl);
        }
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const config_helper& helper)
    {
        return os << helper.m_impl->table;
    }

    std::string_view config_helper::source() const noexcept
    {
        auto const& table = m_impl->table;
        auto const& region = table.source();
        return region.path ? *region.path : std::string_view("");
    }

    std::optional<bool> config_helper::get_boolean(std::string_view section, std::string_view key) const noexcept
    {
        return get_value<bool>(section, key);
    }

    std::optional<std::int64_t> config_helper::get_integer(std::string_view section, std::string_view key) const noexcept
    {
        return get_value<std::int64_t>(section, key);
    }

    std::optional<std::string_view> config_helper::get_string(std::string_view section, std::string_view key) const noexcept
    {
        return get_value<std::string_view, std::string>(section, key);
    }

    void config_helper::enumerate_boolean(std::string_view section, types::function_ref<void(std::string_view, bool)> callback) const noexcept
    {
        enumerate<bool>(section, callback);
    }

    void config_helper::enumerate_integer(std::string_view section, types::function_ref<void(std::string_view, std::int64_t)> callback) const noexcept
    {
        enumerate<std::int64_t>(section, callback);
    }

    void config_helper::enumerate_string(std::string_view section, types::function_ref<void(std::string_view, std::string_view)> callback) const noexcept
    {
        enumerate<std::string_view, std::string>(section, callback);
    }

    bool config_helper::exists(std::string_view path) const noexcept
    {
        auto const& table = m_impl->table;;

        return static_cast<bool>(table.at_path(path));
    }

    template<typename T1, typename T2>
    std::optional<T1> config_helper::get_value(std::string_view section, std::string_view key) const noexcept
    {
        auto const& table = m_impl->table;;

        auto const node_view = table[section][key];
        if (auto const ptr = node_view.template as<T2>())
        {
            return ptr->get();
        }
        else
        {
            return std::nullopt;
        }
    }

    template<typename T1, typename T2>
    void config_helper::enumerate(std::string_view section, types::function_ref<void(std::string_view, T1)> callback) const noexcept
    {
        auto const& table = m_impl->table;;

        auto const node_view = table.at_path(section);
        if (auto const ptr = node_view.as_table())
        {
            auto const& table = *ptr;
            for(auto const& [k, v] : table)
            {
                if (auto const p = v.template as<T2>())
                {
                    callback(k, p->get());
                }
            }
        }
    }
}
