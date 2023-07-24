# ihft::compiler

This module contains various useful compile-time constants.

The real cpu loads the data into cache in chunks. `CPU_CACHE_LINE_SIZE` reflects this feature of the processor implementation. Using this constant is useful when writing data structures that are used simultaneously in a multithreaded environment. (See more about a false sharing).
