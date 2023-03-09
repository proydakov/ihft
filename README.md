# IHFT

<span style="display: inline-block;">

[![BSD 3 License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/proydakov/ihft/blob/master/LICENSE)
</span>

IHFT is microframework & laboratory for low latency applications.

# Current building status matrix

| Configuration | Static Runtime | Shared Runtime | ASAN+UBSAN | TSAN |
| :---: | :---: | :---: | :--: | :--: |
| Linux(GCC) | [![Build Status][pass-linux-gcc-x64-static]][ci-linux-gcc-x64-static] | [![Build Status][pass-linux-gcc-x64-shared]][ci-linux-gcc-x64-shared] | [![Build Status][pass-linux-gcc-x64-asan]][ci-linux-gcc-x64-asan] | [![Build Status][pass-linux-gcc-x64-tsan]][ci-linux-gcc-x64-tsan] |
| Linux(Clang) | [![Build Status][pass-linux-clang-x64-static]][ci-linux-clang-x64-static] | [![Build Status][pass-linux-clang-x64-shared]][ci-linux-clang-x64-shared] | [![Build Status][pass-linux-clang-x64-asan]][ci-linux-clang-x64-asan] | [![Build Status][pass-linux-clang-x64-tsan]][ci-linux-clang-x64-tsan] |
| MacOS(Clang) | - | [![Build Status][pass-macos-x64-shared]][ci-macos-x64-shared] | [![Build Status][pass-macos-x64-asan]][ci-macos-x64-asan] | [![Build Status][pass-macos-x64-tsan]][ci-macos-x64-tsan] |

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
  - [x] one 2 many stream pod data queue
  - [x] one 2 many stream moveable object queue
  - [ ] batch reader for all moveable object queues
  - [x] python latency analysis tool
  - [ ] C++20 concepts
  - [ ] documentation with latency histograms
- [x] memory module
  - [x] arena allocator
  - [x] stream fixed pool allocator
  - [x] page allocator for 4KB pages
  - [x] huge page allocator for 2MB & 1GB pages
- [x] types module
  - [x] action result
  - [x] function reference
- [x] misc module
  - [x] simple human-readable TOML config system
  - [x] sigaction based signal handler
- [ ] logger module
  - [x] compile-time formatting pattern check
  - [x] smart serializer for input arguments
  - [ ] simple client usage
  - [ ] non reliable delivery
  - [ ] wait-free implementation
  - [ ] std::format as formatting backend
  - [ ] std::source_location as logging info
- [ ] network module
  - [x] multicast demo
  - [x] udp demo
  - [ ] DPDK demo
- [x] engine module
  - [x] logical cpu
  - [x] task assignment
  - [x] configuration verification
  - [x] intelligent component loading
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
  - [ ] regressive run of benchmarks for different CPUs/compilers
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

[Intel Intrinsics Guide](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)

[Agner Fog’s instruction tables](https://agner.org/optimize/instruction_tables.pdf)

[WikiChip CPU information](https://en.wikichip.org/wiki/WikiChip:welcome)

[Intel hardware information](https://ark.intel.com)

[AMD hardware information](https://www.amd.com/ru/products/epyc-server)

[TOML config file format](https://toml.io/en/)

[TCP/IP stack recommendations](http://mails.dpdk.org/archives/dev/2022-September/249265.html)

# DevOps Links

[Osnoise tracer](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/trace/osnoise-tracer.rst/)

[Disable cpu frecuency scaling](https://nixcp.com/disable-cpu-frecuency-scaling/)

[Eric Rigtorp Low Latency Tuning](https://rigtorp.se/low-latency-guide/)

[Red Hat Enterprise Linux-7 Low Latency Tuning](https://access.redhat.com/sites/default/files/attachments/201501-perf-brief-low-latency-tuning-rhel7-v2.1.pdf)

[Red Hat Enterprise Linux-7 Performance Tuning Guide](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/pdf/performance_tuning_guide/Red_Hat_Enterprise_Linux-7-Performance_Tuning_Guide-en-US.pdf)

# People

[Erik Rigtorp personal blog](https://rigtorp.se)

[Martin Thompson mechanical sympathy](https://mechanical-sympathy.blogspot.com)
