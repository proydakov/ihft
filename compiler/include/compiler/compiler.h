#pragma once

// https://rigtorp.se/iife/
#define IHFT_COLD __attribute__((noinline, cold))

// https://clang.llvm.org/docs/AttributeReference.html#noinline
#define IHFT_NOINLINE __attribute__((noinline))

// Lets Clang warn (-Wdangling, an error under -Werror) when a function_ref is
// initialized from a temporary callable that does not outlive the function_ref,
// while still allowing a temporary to be passed to a function taking function_ref.
// https://clang.llvm.org/docs/LifetimeSafety.html#lifetimebound
#if defined(__clang__)
#define IHFT_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define IHFT_LIFETIMEBOUND
#endif
