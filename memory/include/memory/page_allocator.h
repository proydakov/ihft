#pragma once

#include "private/mmap_page_allocator.h"

namespace ihft
{

using four_4b_page_allocator = mmap_page_allocator<1u << 12u, false>;

}
