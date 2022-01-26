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
// Configuration data is immutable after loading
class config_helper final
{
public:
#if defined(__clang__)
    static constexpr size_t SIZE = 72;
#elif defined(__GNUC__) || defined(__GNUG__)
    static constexpr size_t SIZE = 96;
#else
#    error "Unsupported compiler"
#endif
    static constexpr size_t ALIGN = 8;

    using table_storage = std::aligned_storage_t<SIZE, ALIGN>;

    static config_result parse(std::string_view file_path) noexcept;

    ~config_helper();

    config_helper(config_helper const&);
    config_helper(config_helper&&) noexcept;

    config_helper& operator=(config_helper const&) = delete;
    config_helper& operator=(config_helper&&) noexcept = delete;

    friend std::ostream& operator<<(std::ostream&, const config_helper&);

    std::optional<bool> get_boolean(std::string_view section, std::string_view key) const noexcept;
    std::optional<std::int64_t> get_integer(std::string_view section, std::string_view key) const noexcept;
    std::optional<std::string> get_string(std::string_view section, std::string_view key) const noexcept;

private:
    template<typename T>
    std::optional<T> get_value(std::string_view section, std::string_view key) const noexcept;

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
