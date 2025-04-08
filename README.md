# Bench Cplusplus

Welcome to `Bench Cplusplus`, a repository dedicated to benchmarking various aspects of C++ code, including performance, memory usage, and efficiency of different programming constructs. This project is designed to help C++ developers understand the trade-offs of different approaches and optimize their applications.

## Overview

`Bench Cplusplus` is a minimalistic benchmarking project designed to help developers measure the execution time of common C++ code snippets and functions.


## Software Requirements

- A modern C++ compiler supporting C++17 or higher:
- CMake 3.16 or higher 

## Installation

1. Clone the repository:

```bash
git clone https://github.com/allvphx/bench-cplusplus.git
```

2. Build the project:

```bash
mkdir build
cd build
cmake ..
make
```

3. Run benchmarks

```shell
./cplusplus_efficiency
taskset -c 0 ./cplusplus_efficiency
```

## Benchmark APIs

For more details about the benchmarking functions, please refer to:
- [Function Call](./notes/function_call.md): complexity v.s. efficiency (virtual and CDPR), binary size v.s. efficiency (non-inline and inline), flexibility v.s. efficiency (indirect calls and regular calls, CDPR and virtual).
