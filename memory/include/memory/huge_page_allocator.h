#pragma once

#include "private/mmap_page_allocator.h"

namespace ihft
{

template<typename T = std::byte>
using one_gb_huge_page_allocator = impl::mmap_page_allocator<T, impl::_1gb_, true>;

template<typename T = std::byte>
using two_mb_huge_page_allocator = impl::mmap_page_allocator<T, impl::_2mb_, true>;

}
