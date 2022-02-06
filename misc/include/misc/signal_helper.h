#pragma once

//
// This code is based on some other works:
//
// https://habr.com/ru/post/141206/
// https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-sigaction-examine-change-signal-action
//

#include <csignal>
#include <optional>
#include <initializer_list>

namespace ihft::misc
{

using sa_sigaction_t = void (*)(int, siginfo_t*, void*);

bool setup_sigaction_handler(sa_sigaction_t, std::initializer_list<int> signals, std::optional<int> flags = std::nullopt);

bool block_application_signals(std::initializer_list<int> signals);
bool block_thread_signals(std::initializer_list<int> signals);

}

#include "private/signal_helper.inl"
