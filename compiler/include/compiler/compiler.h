#pragma once

#define IHFT_NOINLINE __attribute__((noinline))
#define IHFT_COLD __attribute__((noinline, cold))
