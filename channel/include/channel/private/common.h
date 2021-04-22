#pragma once

#include <constant/constant.h>

#include <cstdint>

namespace ihft::channel
{

struct queue_helper
{
    constexpr static std::size_t to2pow(std::size_t n) noexcept
    {
        constexpr std::size_t min_valid_pow = 2;
        if (0 == n)
        {
            return min_valid_pow;
        }

        constexpr auto max_valid_pow = std::size_t(2) << 30;
        if (n >= max_valid_pow)
        {
            return max_valid_pow;
        }

        std::size_t power = min_valid_pow;
        while(power < n)
        {
            power *= 2;
        }
        return std::min(max_valid_pow, power);
    }
};

} // ihft::channel
