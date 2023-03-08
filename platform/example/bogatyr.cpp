#include <platform/platform.h>

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

[[maybe_unused]] inline constexpr const char * const russian_bogatyrs[] = {
    "Добрыня Никитич",
    "Илья Муромец",
    "Алёша Попович"
};

int main()
{
    std::vector<std::thread> threads;

    for(size_t i = 0; i < std::size(russian_bogatyrs); i++)
    {
        threads.emplace_back([i](){
            auto const ptr = russian_bogatyrs[i];
            ihft::platform::trait::set_current_thread_name(ptr);

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
