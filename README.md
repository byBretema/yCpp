# BaseCpp

> A tiny start point for your next C++ project

- Batteries included:
    - Bee - A set of utilities making C++ more comfy.
    - LightPM - A CMake wrapper to simplify external deps.

- Folders structure:
    - src
        - **whatever/**
        - **you/**
        - **need/**
        - CMakeLists.txt
        - main.cpp *(if executable)*
    - vendor *(no-project-related-code)*
    - .clang-format *(please)*
    - CMakeLists.txt
    - run.py

- Usage:
    1. Place your source files in the `src` folder.
    2. Modify your dependencies on the top level `CMakeLists.txt`
    3. Other code not related to project add them to `vendor`
    4. System level deps:
        - Python 3.x
        - CMake
        - Ninja *(optional but recommended)*
    5. Run
        ```shell
        pyhton run.py -c -r
        # For detailed params : pyhton build.py -h
        ```
    6. Enjoy !
        - *If your code compiles* :sweat_smile:
