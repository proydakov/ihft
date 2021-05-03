#pragma once

#include "private/mmap_page_allocator.h"

namespace ihft
{

using one_gb_huge_page_allocator = impl::mmap_page_allocator<impl::_1gb_, true>;

using two_mb_huge_page_allocator = impl::mmap_page_allocator<impl::_2mb_, true>;

}
