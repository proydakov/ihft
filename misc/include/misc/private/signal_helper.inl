#pragma once

#include <cstring>

namespace ihft::misc
{

[[nodiscard("Please check change signal handler result")]]
bool setup_sigaction_handler(sa_sigaction_t action, std::initializer_list<int> signals, std::optional<int> flags)
{
    struct sigaction newhandler;
    memset(&newhandler, 0, sizeof(newhandler));

    newhandler.sa_sigaction = action;

    if(flags)
    {
        newhandler.sa_flags = *flags;
    }


    sigemptyset(&newhandler.sa_mask);
    for(auto const signal : signals)
    {
        sigaddset(&newhandler.sa_mask, signal);
    }

    bool succeeded = true;
    for(auto const signal : signals)
    {
        // https://man7.org/linux/man-pages/man2/sigaction.2.html
        succeeded &= sigaction(signal, &newhandler, nullptr) == 0;
    }

    return succeeded;
}

[[nodiscard("Please check change signal mask for application result")]]
bool block_application_signals(std::initializer_list<int> signals)
{
    sigset_t mask;
    sigemptyset(&mask);

    for(auto const signal : signals)
    {
        sigaddset(&mask, signal);
    }

    // https://man7.org/linux/man-pages/man2/sigprocmask.2.html
    return sigprocmask(SIG_BLOCK, &mask, nullptr) == 0;
}

[[nodiscard("Please check change signal mask for thread result")]]
bool block_thread_signals(std::initializer_list<int> signals)
{
    sigset_t mask;
    sigemptyset(&mask);

    for(auto const signal : signals)
    {
        sigaddset(&mask, signal);
    }

    // https://man7.org/linux/man-pages/man3/pthread_sigmask.3.html
    return pthread_sigmask(SIG_BLOCK, &mask, nullptr) == 0;
}

}
