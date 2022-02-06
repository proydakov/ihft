#include "catch2/catch.hpp"

#include "misc/signal_helper.h"

using namespace ihft::misc;

void mysa_sigaction(int signal, siginfo_t* info, void* context)
{
    printf("In mysa_sigaction: %s info: %p context: %p.\n", strsignal(signal), (void*)info, (void*)context);
}

TEST_CASE("setup_sigaction_handler")
{
    REQUIRE(setup_sigaction_handler(&mysa_sigaction, {SIGUSR1, SIGUSR2}));
}

TEST_CASE("block_application_signals")
{
    REQUIRE(block_application_signals({SIGINT}));
}

TEST_CASE("block_thread_signals")
{
    REQUIRE(block_thread_signals({SIGINT}));
}
