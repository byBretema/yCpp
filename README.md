# y-cpp

> A tiny start point for your next C++ project

- 🔋 Batteries included:
    - y.cmake - A CMake wrapper to simplify external deps.
    - y.hpp - A set of utilities making C++ more comfy.
    - y.py - A set of utilities for python.

- 🌳 Root tree:
    - **project-1**
        - whatever/
        - you/
        - need/
        - CMakeLists.txt
        - main.cpp *(if executable)*
    - **project-2**
        - ...
        - CMakeLists.txt
        - main.cpp *(if executable)*
    - **tests**
        - test.cpp
        - your.cpp
        - code.cpp
    - **vendor**
        - *batteries*
        - *external code*
    - .clang-format *(please)*
    - CMakeLists.txt
    - run.py


- 📚️ Usage:
    1. Place your source files in a new folder *(i.e. `project-x` folder)*.
    2. Modify your dependencies on the top level `CMakeLists.txt`
    3. Any other external code goes into `vendor`
    4. System deps:
        - Python 3.x
        - CMake
        - Ninja *(optional but recommended)*
    5. Run
        ```shell
        pyhton run.py -cbrt
        # For detailed params : pyhton run.py -h
        ```
    6. Enjoy !
        - *If your code compiles* :sweat_smile:
    7. Output example
        ```
        build/
        ├── bin/
        │   └── project-x/
        │       └── project-x
        ├── deps/
        │   ├── argparse-*/
        │   │   └── ...
        │   ├── glm-*/
        │   │   └── ...
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

