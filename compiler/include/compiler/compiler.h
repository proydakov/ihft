#pragma once

// https://rigtorp.se/iife/
#define IHFT_COLD __attribute__((noinline, cold))

// https://clang.llvm.org/docs/AttributeReference.html#noinline
#define IHFT_NOINLINE __attribute__((noinline))
