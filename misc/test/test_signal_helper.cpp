#include <catch2/catch_test_macros.hpp>

#include <misc/signal_helper.h>

#include <atomic>

using namespace ihft::misc;

void mysa_sigaction(int signal, siginfo_t* info, void* context)
{
    printf("In mysa_sigaction: %s info: %p context: %p.\n", strsignal(signal), (void*)info, (void*)context);
}

TEST_CASE("setup_sigaction_handler")
{
    REQUIRE(setup_sigaction_handler(&mysa_sigaction, {SIGUSR1, SIGUSR2}));
}

namespace
{

std::atomic<int> g_caught_signal{0};
std::atomic<int> g_info_signo{-1};

void siginfo_sigaction(int signal, siginfo_t* info, void*)
{
    g_caught_signal.store(signal, std::memory_order_relaxed);
    if (info != nullptr)
    {
        g_info_signo.store(info->si_signo, std::memory_order_relaxed);
    }
}

}

TEST_CASE("setup_sigaction_handler delivers siginfo")
{
    REQUIRE(setup_sigaction_handler(&siginfo_sigaction, {SIGUSR1}));

    REQUIRE(raise(SIGUSR1) == 0);

    REQUIRE(g_caught_signal.load(std::memory_order_relaxed) == SIGUSR1);
    REQUIRE(g_info_signo.load(std::memory_order_relaxed) == SIGUSR1);

    struct sigaction restore{};
    restore.sa_handler = SIG_DFL;
    sigemptyset(&restore.sa_mask);
    REQUIRE(sigaction(SIGUSR1, &restore, nullptr) == 0);
}

TEST_CASE("block_application_signals")
{
    REQUIRE(block_application_signals({SIGINT}));
}

TEST_CASE("block_thread_signals")
{
    REQUIRE(block_thread_signals({SIGINT}));
}
