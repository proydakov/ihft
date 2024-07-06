# IHFT

<span style="display: inline-block;">

[![BSD 3 License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/proydakov/ihft/blob/master/LICENSE)
</span>

> Life is too short for malloc.

IHFT is microframework & laboratory for low latency applications.

# System requirements

The IHFT framework requires:

- modern C++ compiler with -std=20 support (clang16+, gcc11+)
- cmake build system generator
- ninja build command line utility
- modern computer with linux x64 system
- dev compilation on macos x64|arm64 is also available

# Current building status matrix

| Configuration | Static Runtime | Shared Runtime | ASAN+UBSAN | TSAN |
| :---: | :---: | :---: | :--: | :--: |
| Linux(GCC11) | [![Build Status][pass-linux-gcc-x64-static]][ci-linux-gcc-x64-static] | [![Build Status][pass-linux-gcc-x64-shared]][ci-linux-gcc-x64-shared] | [![Build Status][pass-linux-gcc-x64-asan]][ci-linux-gcc-x64-asan] | [![Build Status][pass-linux-gcc-x64-tsan]][ci-linux-gcc-x64-tsan] |
| Linux(Clang18) | [![Build Status][pass-linux-clang-x64-static]][ci-linux-clang-x64-static] | [![Build Status][pass-linux-clang-x64-shared]][ci-linux-clang-x64-shared] | [![Build Status][pass-linux-clang-x64-asan]][ci-linux-clang-x64-asan] | [![Build Status][pass-linux-clang-x64-tsan]][ci-linux-clang-x64-tsan] |
| MacOS(Clang15) | - | [![Build Status][pass-macos-x64-shared]][ci-macos-x64-shared] | [![Build Status][pass-macos-x64-asan]][ci-macos-x64-asan] | [![Build Status][pass-macos-x64-tsan]][ci-macos-x64-tsan] |

[pass-linux-gcc-x64-static]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-gcc-x64-static.yml?branch=master
[pass-linux-gcc-x64-shared]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-gcc-x64.yml?branch=master
[pass-linux-gcc-x64-asan]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-gcc-x64-asan-ubsan.yml?branch=master
[pass-linux-gcc-x64-tsan]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-gcc-x64-tsan.yml?branch=master
[pass-linux-clang-x64-static]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-clang-x64-static.yml?branch=master
[pass-linux-clang-x64-shared]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-clang-x64.yml?branch=master
[pass-linux-clang-x64-asan]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-clang-x64-asan-ubsan.yml?branch=master
[pass-linux-clang-x64-tsan]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/linux-clang-x64-tsan.yml?branch=master
[pass-macos-x64-shared]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/macos-x64.yml?branch=master
[pass-macos-x64-asan]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/macos-x64-asan-ubsan.yml?branch=master
[pass-macos-x64-tsan]: https://img.shields.io/github/actions/workflow/status/proydakov/ihft/macos-x64-tsan.yml?branch=master

[ci-linux-gcc-x64-static]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-gcc-x64-static
[ci-linux-gcc-x64-shared]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-gcc-x64
[ci-linux-gcc-x64-asan]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-gcc-x64-asan-ubsan
[ci-linux-gcc-x64-tsan]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-gcc-x64-tsan
[ci-linux-clang-x64-static]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-clang-x64-static
[ci-linux-clang-x64-shared]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-clang-x64
[ci-linux-clang-x64-asan]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-clang-x64-asan-ubsan
[ci-linux-clang-x64-tsan]: https://github.com/proydakov/ihft/actions?query=workflow%3Alinux-clang-x64-tsan
[ci-macos-x64-shared]: https://github.com/proydakov/ihft/actions?query=workflow%3Amacos-x64
[ci-macos-x64-asan]: https://github.com/proydakov/ihft/actions?query=workflow%3Amacosx64-asan-ubsan
[ci-macos-x64-tsan]: https://github.com/proydakov/ihft/actions?query=workflow%3Amacos-x64-tsan

# Goals

- [x] platform module
  - [x] set cpu for current thread
  - [x] set name for current thread
  - [x] lock memory pages
  - [x] status for isolcpus, nohz_full, rcu_nocbs
  - [x] hyper-threading detector
  - [x] cpu frequency scaling detector
  - [x] swap detector
  - [x] transparent huge pages detector
  - [x] huge page system detector
  - [x] core to core latency example
  - [x] platform sysjitter example
- [x] channel module
  - [x] one 2 one stream pod data queue
  - [x] one 2 one stream moveable object queue
  - [ ] one 2 many stream pod data queue
  - [ ] one 2 many stream moveable object queue
  - [x] one 2 each stream pod data queue
  - [x] one 2 each stream moveable object queue
  - [ ] batch reader for all moveable object queues
  - [x] python latency analysis tool
  - [x] C++20 concepts
  - [x] documentation with latency histograms
- [x] memory module
  - [x] arena allocator
  - [x] stream fixed pool allocator
  - [x] page allocator for 4KB pages
  - [x] huge page allocator for 2MB & 1GB pages
- [x] timer module
  - [x] native cpu_counter
  - [x] native cpu_pause
- [x] types module
  - [x] box: placed object ownership
  - [x] function_ref: non-owning reference to a callable
  - [x] result: function output or error
- [x] misc module
  - [x] simple human-readable TOML config system
  - [x] sigaction based signal handler
- [x] logger module
  - [x] simple client usage
  - [x] non reliable delivery
  - [x] smart serializer for input arguments
  - [x] compile-time formatting pattern check
  - [x] wait-free implementation
  - [x] log pount source file and thread info
  - [ ] std::format as formatting backend
  - [x] std::source_location as log point source
  - [ ] documentation with pictures
- [ ] network module
  - [x] udp demo
  - [x] multicast demo
  - [ ] DPDK demo
- [x] engine module
  - [x] logical cpu
  - [x] task assignment
  - [x] configuration verification
  - [x] intelligent component loading
  - [x] documentation with pictures
- [x] CI
  - [x] github workflows CI
  - [x] static binary
  - [x] Clang support with libc++
  - [x] GCC support with libstdc++
  - [x] Catch2 unit tests
  - [x] address sanitizer
  - [x] thread sanitizer
  - [x] ub sanitizer
  - [ ] clang format
- [x] DevOps
  - [x] script for system latency optimization

# Docs

[Analytics](analytics/README.md)

[Channel](channel/README.md)

[Compiler](compiler/README.md)

[Constant](constant/README.md)

[Engine](engine/README.md)

[Memory](memory/README.md)

[Misc](misc/README.md)

[Platform](platform/README.md)

[Types](types/README.md)

# Dev Links

[C++ reference](https://en.cppreference.com/w/)

[Compiler Explorer](https://godbolt.org)

[Compiler Flags and Options](https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html)

[Agner Fog’s instruction tables](https://agner.org/optimize/instruction_tables.pdf)

[Intel Intrinsics Guide](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)

[WikiChip CPU information](https://en.wikichip.org/wiki/WikiChip:welcome)

[Intel hardware information](https://ark.intel.com)

[AMD hardware information](https://www.amd.com/ru/products/epyc-server)

[LLVM Machine Code Analyzer](https://llvm.org/docs/CommandGuide/llvm-mca.html)

[LIKWID tools](https://github.com/RRZE-HPC/likwid/wiki)

[OpenOnload is a user-level network stack](https://github.com/Xilinx-CNS/onload)

[DPDK Accelerating Network Performance](https://www.dpdk.org)

[TOML config file format](https://toml.io/en/)

# DevOps Links

[Osnoise tracer](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/trace/osnoise-tracer.rst/)

[Disable cpu frecuency scaling](https://nixcp.com/disable-cpu-frecuency-scaling/)

[Eric Rigtorp Low Latency Tuning](https://rigtorp.se/low-latency-guide/)

[Red Hat Enterprise Linux-7 Low Latency Tuning](https://access.redhat.com/sites/default/files/attachments/201501-perf-brief-low-latency-tuning-rhel7-v2.1.pdf)

[Red Hat Enterprise Linux-7 Performance Tuning Guide](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/pdf/performance_tuning_guide/Red_Hat_Enterprise_Linux-7-Performance_Tuning_Guide-en-US.pdf)

# People

[Erik Rigtorp personal blog](https://rigtorp.se)

[Martin Thompson mechanical sympathy](https://mechanical-sympathy.blogspot.com)

[Raymond Chen oldnewthing](https://devblogs.microsoft.com/oldnewthing/)

[Daniel Lemire's blog](https://lemire.me/blog/)
