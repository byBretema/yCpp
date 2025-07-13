# Copyright Daniel Brétema, 2025.
# Distributed under the Boost Software License, Version 1.0.
# See complete details at https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.28)
include(vendor/LightPM.cmake)

###############################################################################
## Project !

get_filename_component(PName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PName}_TOP)

###############################################################################
## Properties !

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###############################################################################
## Deps !

LPM_AddPkg( "fmt"       "11.1.4"  "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip" )
LPM_AddPkg( "glm"       "1.0.1"   "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"             )
LPM_AddPkg( "argparse"  "3.2"     "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip"        )

###############################################################################
## Compile !

include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
add_subdirectory(src)
add_subdirectory(tests)
