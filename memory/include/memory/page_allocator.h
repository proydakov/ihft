#pragma once

#include "private/mmap_page_allocator.h"

namespace ihft
{

using four_4b_page_allocator = impl::mmap_page_allocator<impl::_4kb_, false>;

}
