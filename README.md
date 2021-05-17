# IHFT

<span style="display: inline-block;">

[![Travis CI Status](https://travis-ci.org/proydakov/ihft.svg?branch=master)](https://travis-ci.org/proydakov/ihft)
[![BSD 3 License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/proydakov/ihft/blob/master/LICENSE)
</span>

IHFT is microframework & laboratory for low latency applications.

# Goals

- [x] platform module
  - [x] set cpu for current thread
  - [x] set name for current thread
  - [x] status for isolcpus, nohz_full, rcu_nocbs
  - [x] hyper-threading detector
  - [x] cpu frequency scaling detector
  - [x] swap detector
  - [x] transparent huge pages detector
  - [x] huge page system detector
- [x] channel module
  - [x] one 2 one stream pod data queue
  - [x] one 2 one stream moveable object queue
  - [x] one 2 many stream pod data queue
  - [x] one 2 many stream moveable object queue
  - [ ] next node consume bucket r&d
  - [ ] batch reader for all moveable object queues
  - [x] python latency analysis tool
  - [ ] add latency histograms in documentation
- [ ] memory module
  - [ ] arena allocator
  - [x] stream fixed pool allocator
  - [x] page allocator for 4KB pages
  - [x] huge page allocator for 2MB & 1GB pages
- [ ] misc module
  - [ ] simple human-readable TOML config system
- [ ] logger module
  - [ ] simple client usage
  - [ ] non reliable delivery
  - [ ] lock-free implementation
- [ ] network module
  - [ ] multicast demo
  - [ ] DPDK demo
- [ ] engine module
  - [ ] logical cpu
  - [ ] cpu assignment
  - [ ] configuration verification
- [x] CI & DevOps
  - [x] travis CI
  - [x] static binary
  - [x] Clang support with libc++
  - [x] GCC support with libstdc++
  - [x] Catch2 unit tests
  - [x] address sanitizer
  - [x] thread sanitizer
  - [x] ub sanitizer
- [x] DevOps
  - [x] script for system latency optimization

# Dev Links

[C++ reference](https://en.cppreference.com/w/)

[Compiler Explorer](https://godbolt.org)

[Intel Intrinsics Guide](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)

[Agner Fog’s instruction tables](https://agner.org/optimize/instruction_tables.pdf)

[WikiChip CPU information](https://en.wikichip.org/wiki/WikiChip:welcome)

[Intel hardware information](https://ark.intel.com)

[AMD hardware information](https://www.amd.com/ru/products/epyc-server)

[TOML config file format](https://toml.io/en/)

# DevOps Links

[Disable cpu frecuency scaling](https://nixcp.com/disable-cpu-frecuency-scaling/)

[Eric Rigtorp Low Latency Tuning](https://rigtorp.se/low-latency-guide/)

[Red Hat Enterprise Linux-7 Low Latency Tuning](https://access.redhat.com/sites/default/files/attachments/201501-perf-brief-low-latency-tuning-rhel7-v2.1.pdf)

[Red Hat Enterprise Linux-7 Performance Tuning Guide](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/pdf/performance_tuning_guide/Red_Hat_Enterprise_Linux-7-Performance_Tuning_Guide-en-US.pdf)

# People

[Erik Rigtorp personal blog](https://rigtorp.se)

[Martin Thompson mechanical sympathy](https://mechanical-sympathy.blogspot.com)
