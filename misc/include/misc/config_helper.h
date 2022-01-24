#pragma once

#include <iosfwd>
#include <string>
#include <optional>
#include <type_traits>
#include <string_view>

namespace ihft::misc
{

class config_result;

// This class provides a simple config assist
class config_helper final
{
public:
    static constexpr size_t SIZE = 72;
    static constexpr size_t ALIGN = 8;
    using table_storage = std::aligned_storage_t<SIZE, ALIGN>;

    static config_result parse(std::string_view file_path) noexcept;

    ~config_helper();

    config_helper(config_helper const&);
    config_helper(config_helper&&) noexcept;

    config_helper& operator=(config_helper const&) = delete;
    config_helper& operator=(config_helper&&) noexcept = delete;

    friend std::ostream& operator<<(std::ostream&, const config_helper&);

private:
    config_helper() noexcept;

    template<typename T>
    static config_result parse_impl(T file_path);

private:
    table_storage m_table;
};

// This class contains config_helper or error
class config_result final
{
public:
    config_result(std::string error) : m_error(std::move(error))
    {
    }

    config_result(config_helper helper) : m_config_helper(std::move(helper))
    {
    }

    // Returns true if parsing succeeeded.
    bool succeeded() const noexcept
    {
        return m_error.empty();
    }

    // Returns true if parsing failed.
    bool failed() const noexcept
    {
        return not succeeded();
    }

    operator bool() const noexcept
    {
        return succeeded();
    }

    config_helper& value() & noexcept
    {
        return m_config_helper.value();
    }

    const config_helper& value() const & noexcept
    {
        return m_config_helper.value();
    }

    config_helper&& value() && noexcept
    {
        return std::move(m_config_helper).value();
    }

    const config_helper&& value() const && noexcept
    {
        return std::move(m_config_helper).value();
    }

    const std::string& error() const noexcept
    {
        return m_error;
    }

    friend std::ostream& operator<<(std::ostream&, const config_result&);

private:
    std::string m_error;
    std::optional<config_helper> m_config_helper;
};

}
