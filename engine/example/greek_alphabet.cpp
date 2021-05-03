#include <platform/platform.h>

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

[[maybe_unused]] inline constexpr const char * const greek_alphabet_ascii[] = {
    "alpha",
    "beta",
    "gamma",
    "delta",
    "epsilon",
    "zeta",
    "eta",
    "theta",
    "iota",
    "kappa",
    "lamda",
    "mu",
    "nu",
    "xi",
    "omicron",
    "pi",
    "rho",
    "sigma",
    "tau",
    "upsilon",
    "phi",
    "chi",
    "psi",
    "omega"
};

[[maybe_unused]] inline constexpr const char * const greek_alphabet_utf8[] = {
    "\u03B1",
    "\u03B2",
    "\u03B3",
    "\u03B4",
    "\u03B5",
    "\u03B6",
    "\u03B7",
    "\u03B8",
    "\u03B9",
    "\u03BA",
    "\u03BB",
    "\u03BC",
    "\u03BD",
    "\u03BE",
    "\u03BF",
    "\u03C0",
    "\u03C1",
    "\u03C3",
    "\u03C4",
    "\u03C5",
    "\u03C6",
    "\u03C7",
    "\u03C8",
    "\u03C9"
};

static_assert(std::size(greek_alphabet_ascii) == std::size(greek_alphabet_utf8));

int main()
{
    std::vector<std::thread> threads;

    for(size_t i = 0; i < std::size(greek_alphabet_utf8); i++)
    {
        threads.emplace_back([i](){
            auto const ptr = greek_alphabet_utf8[i];
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
