# util-linux-cpp

A Header-only library that makes it easy to use Linux utility commands.

---

## Contents

- [util-linux-cpp](#util-linux-cpp)
  - [Contents](#contents)
  - [Dependencies](#dependencies)
  - [Features](#features)
    - [lsblk](#lsblk)
      - [Examples](#examples)
  - [Usage](#usage)
    - [CMake](#cmake)
      - [CMakeLists.txt](#cmakeliststxt)
  - [Test](#test)

---

## Dependencies

- [nlohmann-json](https://github.com/nlohmann/json.git)
- [ext](https://github.com/ntoskrnl7/ext)

---

## Features

### lsblk

Supported lsblk command versions: 2.29.2 or later
(Test on 2.29.2 and 2.33.1)

#### Examples

```cpp
#include <util_linux/lsblk.hpp>

nlohmann::json result = util_linux::lsblk();
std::cout << result.dump(4) << std::endl;

util_linux::lsblk("/dev/sda");
```

---

## Usage

### CMake

#### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# create project
project(MyProject)

# add executable
add_executable(tests tests.cpp)

# add dependencies
include(cmake/CPM.cmake)
CPMAddPackage("gh:ntoskrnl7/util-linux-cpp@0.1.1")

# link dependencies
target_link_libraries(tests util-linux)
```

---

## Test

```bash
cd tests
mkdir build && cd build
cmake ..
cmake --build .
./unittest
```
