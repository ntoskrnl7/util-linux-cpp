# util-linux-cpp

A Header-only library that makes it easy to use Linux utility commands.

## Dependencies

[nlohmann-json](https://github.com/nlohmann/json.git)
[ext](https://github.com/ntoskrnl7/ext)

## Features

### lsblk

Supported lsblk command versions: 2.29.2 or later
(Test on 2.29.2 and 2.33.1)

#### Example

```cpp
#include <util_linux/lsblk.hpp>

nlohmann::json result = util_linux::lsblk();
std::cout << result.dump(4) << std::endl;

util_linux::lsblk("/dev/sda");
```

## Test

```bash
mkdir build && cd build
cmake ..
cmake --build .
./util-linux-cpp-test
```
