# y-cpp

> A tiny start point for your next C++ project

## 🔋 Project

- 🔵 **_[y.hpp](docs/y_cpp.md)_** - A set of utilities making C++ more comfy

- 🐍 **_y.py_** - A common set of utilities for python

- 📦️ **_y.cmake_** - Simplify external deps

## 📚️ Usage

1. Place your source files in a new folder _(i.e. `project-x` folder)_.
2. Modify your dependencies on the top level `CMakeLists.txt`
3. Any other external code goes into `vendor`
4. System deps: `Python > 3.10`, `CMake > 3.28`, `Ninja` _(optional but recommended)_
5. Run: `./run -cbrt`
6. Enjoy!

## 📂 Structure

### 🌳 Example source tree

```
.
├── project-1/
│   ├── whatever/
│   ├── you/
│   ├── need/
│   ├── CMakeLists.txt
│   └── main.cpp (if executable)
├── project-2/
│   ├── ...
│   ├── CMakeLists.txt
│   └── main.cpp (if executable)
├── tests/
│   ├── test.cpp
│   ├── your.cpp
│   └── code.cpp
├── vendor/
│   ├── y.cmake
│   ├── y.cpp
│   ├── y.py
│   └── ... other external code
├── .clang-format (please)
├── CMakeLists.txt
└── run

```

### 🌳 Example build tree

```
build/
├── bin/
│   └── project-x/
│       └── project-x
├── deps/
│   ├── argparse-*/
│   │   └── ...
│   └── glm-*/
│       └── ...
├── sub-build/
│   ├── CMakeFiles/
│   │   └── ...
│   ├── project-x/
│   │   └── ...
│   ├── .ninja_deps
│   ├── .ninja_log
│   ├── CMakeCache.txt
│   ├── CPackConfig.cmake
│   ├── CPackSourceConfig.cmake
│   ├── build.ninja
│   └── cmake_install.cmake
└── tests/
    ├── y_core
    └── y_libs
```
