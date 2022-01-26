#include <misc/config_helper.h>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.h>

#include <memory>
#include <sstream>
#include <ostream>

namespace
{
    toml::table& ref(ihft::misc::config_helper::table_storage& storage) noexcept
    {
        return *std::launder(reinterpret_cast<toml::table*>(&storage));
    }

    toml::table const& cref(ihft::misc::config_helper::table_storage const& storage) noexcept
    {
        return *std::launder(reinterpret_cast<const toml::table*>(&storage));
    }

    toml::table&& borrow(ihft::misc::config_helper::table_storage& storage) noexcept
    {
        return std::move(*std::launder(reinterpret_cast<toml::table*>(&storage)));
    }
}

namespace ihft::misc
{
    template<typename T>
    ihft::misc::config_result config_helper::parse_impl(T file_path)
    {
        auto config = toml::parse_file(file_path);

        if (config)
        {
            ihft::misc::config_helper helper;
            std::construct_at(reinterpret_cast<toml::table*>(&helper.m_table), std::move(config).table());
            return helper;
        }
        else
        {
            std::stringstream sstream;
            sstream << config.error();
            return sstream.str();
        }
    }

    config_result config_helper::parse(std::string_view file_path) noexcept
    {
        return parse_impl(file_path);
    }

    config_helper::config_helper() noexcept
    {
        static_assert(SIZE == sizeof(toml::table));
        static_assert(ALIGN == alignof(toml::table));
    }

    config_helper::~config_helper()
    {
        std::destroy_at(&ref(m_table));
    }

    config_helper::config_helper(const config_helper& other)
    {
        std::construct_at(reinterpret_cast<toml::table*>(&m_table), cref(other.m_table));
    }

    config_helper::config_helper(config_helper&& other) noexcept
    {
        std::construct_at(reinterpret_cast<toml::table*>(&m_table), borrow(other.m_table));
    }

    std::ostream& operator<<(std::ostream& os, const config_helper& helper)
    {
        return os << cref(helper.m_table);
    }

    std::ostream& operator<<(std::ostream& os, const config_result& result)
    {
        if (result)
        {
            os << result.value();
        }
        else
        {
            os << result.error();
        }
        return os;
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

    template<typename T1, typename T2>
    std::optional<T1> config_helper::get_value(std::string_view section, std::string_view key) const noexcept
    {
        auto const& table = cref(m_table);

        auto const node_view = table[section][key];
        if (auto ptr = node_view.as<T2>())
        {
            return ptr->get();
        }
        else
        {
            return std::nullopt;
        }
    }
}
