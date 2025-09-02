cmake_minimum_required(VERSION 3.28)
include(vendor/y.cmake)

y_setup_top_level(20)

y_add_library( "fmt"       "11.1.4"  "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip" OFF)
y_add_library( "glm"       "1.0.1"   "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"             OFF)
y_add_library( "argparse"  "3.2"     "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip"        OFF)


y_get_projects(_projects build tests vendor)

message(STATUS "y · Projects ---> ${_projects}")


# y_enable_tests()
