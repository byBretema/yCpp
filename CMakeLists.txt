set(TopCmakeMinVer 3.28)
cmake_minimum_required(VERSION ${TopCmakeMinVer})

include(vendor/y.cmake)

get_filename_component(TopName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${TopName})

y_set_defaults(20)

y_add_external("fmt"       "11.1.4"  "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip")
y_add_external("glm"       "1.0.1"   "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"            )
y_add_external("argparse"  "3.2"     "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip"       )

y_auto_projects(build tests vendor)
y_enable_tests()
