#pragma once

#include <string_view>

namespace ihft::logger::impl
{

struct source_location final
{
    constexpr static source_location current(std::string_view f, std::string_view p, unsigned l)
    {
        return {f, p, l};
    }

    constexpr std::string_view file_name() const
    {
        return m_file;
    }

    constexpr std::string_view function_name() const
    {
        return m_func;
    }

    constexpr unsigned line() const
    {
        return m_line;
    }

    std::string_view m_file{};
    std::string_view m_func{};
    unsigned m_line{};
};

consteval std::string_view prepare_file_path(std::string_view rawfile)
{
    auto const pos = rawfile.rfind("/");
    return std::string_view::npos == pos ?
        rawfile :
        rawfile.substr(pos + 1, rawfile.length() - pos - 1);
}

}

#define source_location_current( ) ::ihft::logger::impl::source_location::current(::ihft::logger::impl::prepare_file_path(__FILE__), __PRETTY_FUNCTION__, __LINE__)
