#pragma once

#include "private/mmap_page_allocator.h"

namespace ihft
{

using one_gb_huge_page_allocator = mmap_page_allocator<1u << 30u, true>;

using two_mb_huge_page_allocator = mmap_page_allocator<1u << 21u, true>;

}
