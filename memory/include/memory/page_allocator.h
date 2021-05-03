#pragma once

#include "private/mmap_page_allocator.h"

namespace ihft
{

template<typename T = std::byte>
using four_4b_page_allocator = impl::mmap_page_allocator<T, impl::_4kb_, false>;

}
