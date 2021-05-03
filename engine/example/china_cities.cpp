#include <platform/platform.h>

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

[[maybe_unused]] inline constexpr const char * const cities_in_china[] = {
    "上海",
    "北京",
    "深圳",
    "香港",
    "广州",
    "重庆",
    "天津",
    "苏州",
    "成都",
    "澳门",
    "台北"
};

int main()
{
    std::vector<std::thread> threads;

    auto const size = std::min<size_t>(std::size(cities_in_china), std::thread::hardware_concurrency());
    for(size_t i = 0; i < size; i++)
    {
        threads.emplace_back([i](){
            auto const ptr = cities_in_china[i];
            ihft::platform::set_current_thread_name(ptr);

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(60s);
        });
    }

    for(auto& t : threads)
    {
        t.join();
    }

    return 0;
}
