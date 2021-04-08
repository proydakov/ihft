# IHFT

<span style="display: inline-block;">

[![Travis CI Status](https://travis-ci.org/proydakov/ihft.svg?branch=master)](https://travis-ci.org/proydakov/ihft)
[![BSD 3 License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/proydakov/ihft/blob/master/LICENSE)
</span>

IHFT is microframework for low latency applications.

# Goals

- [x] platform module
  - [x] set cpu for current thread
  - [x] set name for current thread
  - [x] verification for cpu isolation
- [x] channel module
  - [x] one 2 one pod data queue
  - [x] one 2 one moveable object queue
  - [ ] one 2 one moveable intrusive object queue 
  - [x] one 2 many pod data queue
  - [x] one 2 many moveable object queue
  - [ ] one 2 many moveable intrusive object queue
  - [ ] batch reader for one 2 one & one 2 many moveabel object queue
- [ ] memory module
  - [ ] stream data allocator
  - [ ] 4KB, 2MB & 1GB page allocator
- [ ] task module
  - [ ] logical task
  - [ ] logical thread
  - [ ] cpu settings
- [ ] logger module
  - [ ] simple client usage
  - [ ] non reliable delivery
  - [ ] lock-free implementation
- [ ] configuration module
  - [ ] simple human-readable csv-based config system
- [x] CI & DevOps
  - [x] travis CI
  - [x] static binary
  - [x] address sanitizer

# Dev Links

[C++ reference](https://en.cppreference.com/w/)

[Compiler Explorer](https://godbolt.org)

[Agner Fog’s instruction tables](https://agner.org/optimize/instruction_tables.pdf)

[GCC & Clang Optimizations for Embedded Linux](https://www.linuxsecrets.com/elinux-wiki/images/2/22/GCC_Clang_Optimizations_for_Embedded_Linux.pdf)

[What every programmer should know about memory (pdf version)](https://people.freebsd.org/~lstewart/articles/cpumemory.pdf)

[WikiChip CPU information](https://en.wikichip.org/wiki/WikiChip:welcome)

[Intel hardware information](https://ark.intel.com)

[AMD hardware information](https://www.amd.com/ru/products/epyc-server)

# DevOps Links

[Disable cpu frecuency scaling](https://nixcp.com/disable-cpu-frecuency-scaling/)

[Red Hat Enterprise Linux-7 Low Latency Tuning](https://access.redhat.com/sites/default/files/attachments/201501-perf-brief-low-latency-tuning-rhel7-v2.1.pdf)

[Red Hat Enterprise Linux-7 Performance Tuning Guide](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/pdf/performance_tuning_guide/Red_Hat_Enterprise_Linux-7-Performance_Tuning_Guide-en-US.pdf)

# People

[Erik Rigtorp personal blog](ttps://rigtorp.se)

