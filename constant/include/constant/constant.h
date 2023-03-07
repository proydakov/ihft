#pragma once

//
// @TODO:
//
// use https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size
//

//
// sysctl -a | grep cachelinesize
// hw.cachelinesize: 64
//

namespace ihft::constant
{
    constexpr unsigned CPU_CACHE_LINE_SIZE = 64;
}
