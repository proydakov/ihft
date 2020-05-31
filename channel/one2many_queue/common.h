#pragma once

#include <cstdint>

enum : std::uint64_t { QUEUE_CPU_CACHE_LINE_SIZE = 64 };

struct empty_allocator {};
