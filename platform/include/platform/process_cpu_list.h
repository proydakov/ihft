#pragma once

#include <types/function_ref.h>
#include <string_view>

//
// This code process cpu list from /proc/cmdline & other linux files
// and execute functor: Functor for each cpu in list or range cpus
//
// Example:
// 1,2,3 -> cpus: [1, 2, 3]
// 1-3,5-7 -> cpus: [1, 2, 3, 5, 6, 7]
//
namespace ihft::platform
{

void process_cpu_list(std::string_view cpus, types::function_ref<void(unsigned)> functor);

}
