#pragma once

#include <type_traits>

namespace ihft::channel
{
    template<typename T>
    concept seqnum_counter = std::is_same_v<T, std::uint8_t> ||
                             std::is_same_v<T, std::uint16_t> ||
                             std::is_same_v<T, std::uint32_t> ||
                             std::is_same_v<T, std::uint64_t>
    ;

    template<typename T>
    concept complex_event = std::is_nothrow_move_constructible_v<T>;

    template<typename T>
    concept plain_event = std::is_trivially_copyable_v<T> &&
                          std::is_nothrow_move_constructible_v<T>
    ;
}
