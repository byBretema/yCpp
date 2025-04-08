# cpp-template

> A tiny start point for your next C++ project

1. Place your source files in the `src` folder.

2. Modify your dependencies on the top level `CMakeLists.txt`

3. Or for single-header only add them to `vendor`

4. Install deps:
    - Python 3.x
    - [UV](https://docs.astral.sh/uv/#installation)
    - CMake
    - Ninja *(optional but recommended)*

5. Run
    ```shell
    pyhton build.py [-h] [-c] [-C] [-r] [-g CMAKEGEN] [-a]
    ```
6. Enjoy!  *(if your code properly compiles* :sweat_smile:*)*
