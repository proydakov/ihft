#pragma once

#include <types/result.h>
#include <types/function_ref.h>

#include <iosfwd>
#include <memory>
#include <string>
#include <optional>
#include <type_traits>
#include <string_view>

namespace ihft::misc
{

// This class provides a simple config assist
// Configuration data is immutable after loading
class config_helper final
{
public:
    using config_result = types::result<config_helper, std::string>;

    config_helper(config_helper const&) = delete;
    config_helper(config_helper&&) noexcept;

    config_helper& operator=(config_helper const&) = delete;
    config_helper& operator=(config_helper&&) noexcept;

    static config_result parse(std::string_view file_path);

    ~config_helper();

    friend std::ostream& operator<<(std::ostream&, const config_helper&);

    std::string_view source() const noexcept;

    std::optional<bool> get_boolean(std::string_view section, std::string_view key) const noexcept;
    std::optional<std::int64_t> get_integer(std::string_view section, std::string_view key) const noexcept;
    std::optional<std::string_view> get_string(std::string_view section, std::string_view key) const noexcept;

    void enumerate_boolean(std::string_view section, types::function_ref<void(std::string_view, bool)>) const noexcept;
    void enumerate_integer(std::string_view section, types::function_ref<void(std::string_view, std::int64_t)>) const noexcept;
    void enumerate_string (std::string_view section, types::function_ref<void(std::string_view, std::string_view)>) const noexcept;

    bool exists(std::string_view path) const noexcept;

private:
    template<typename T1, typename T2 = T1>
    std::optional<T1> get_value(std::string_view section, std::string_view key) const noexcept;

    template<typename T1, typename T2 = T1>
    void enumerate(std::string_view section, ihft::types::function_ref<void(std::string_view, T1)>) const noexcept;

    template<typename T>
    config_helper(T) noexcept;

    template<typename T>
    static config_result parse_impl(T file_path);

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

}
