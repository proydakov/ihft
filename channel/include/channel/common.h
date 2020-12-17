#pragma once

#include <cstdint>

namespace ihft
{

constexpr std::size_t QUEUE_CPU_CACHE_LINE_SIZE = 64;

struct empty_allocator {};

} // ihft
