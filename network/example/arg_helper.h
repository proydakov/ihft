#pragma once

#include <charconv>
#include <cstddef>
#include <cstring>
#include <optional>
#include <system_error>

#include <arpa/inet.h>
#include <netinet/in.h>

namespace ihft::network
{

inline std::optional<unsigned short> parse_port(char const* text) noexcept
{
    if (text == nullptr)
    {
        return std::nullopt;
    }

    std::size_t const size = std::strlen(text);

    unsigned int value{};
    auto const [end, ec] = std::from_chars(text, text + size, value);
    if (ec != std::errc() || end != text + size || value == 0 || value > 65535u)
    {
        return std::nullopt;
    }

    return static_cast<unsigned short>(value);
}

inline std::optional<in_addr> parse_ipv4(char const* text) noexcept
{
    if (text == nullptr)
    {
        return std::nullopt;
    }

    in_addr addr{};
    if (::inet_pton(AF_INET, text, &addr) != 1)
    {
        return std::nullopt;
    }

    return addr;
}

} // namespace ihft::network
