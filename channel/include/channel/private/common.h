#pragma once

#include <constant/constant.h>

#include <limits>
#include <cstdint>

namespace ihft::channel
{

struct queue_helper
{
    template<typename T>
    constexpr static std::size_t to2pow(std::size_t n) noexcept
    {
        static_assert(std::is_unsigned_v<T>, "Counter type must be unsigned.");

        constexpr std::size_t min_valid_pow = 4;
        if (n <= min_valid_pow)
        {
            return min_valid_pow;
        }

        constexpr auto max_valid_pow = std::size_t(1) << (sizeof(T) * 8 - 2);
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

// For seqnum-based queues maximum seqnum should be bigger than queue size
static_assert(queue_helper::to2pow<std::uint8_t>(std::numeric_limits<std::uint8_t>::max()) == 64ul);
static_assert(queue_helper::to2pow<std::uint16_t>(std::numeric_limits<std::uint16_t>::max()) == 16384ul);
static_assert(queue_helper::to2pow<std::uint32_t>(std::numeric_limits<std::uint32_t>::max()) == 1073741824ul);
static_assert(queue_helper::to2pow<std::uint64_t>(std::numeric_limits<std::uint64_t>::max()) == 4611686018427387904ul);

} // ihft::channel
