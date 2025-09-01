# y.cmake - A nano wrapper over FetchContent

# Usage:

#     On parent (or project) CMake

#         y_add_dep( "fmt"       "11.1.4" "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip" ON )
#         y_add_dep( "glm"      "1.0.1"   "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"             ON )
#         y_add_dep( "argparse" "3.2"     "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip"        ON )

#         This will populate the ./build/deps dir with given dependencies.
#         The last arg is allow_system.
#             - OFF means 'always download'
#             - ON  means 'try to find it on system'

#     On project CMake

#         y_link_libraries(${PROJECT_NAME})
#             This will link the project with all the libraries added in previous
#             section.

#         y_enable_tests()
#             This will add all the .cpp, .cc or .c in the ./tests dir as tests
#             to later run with 'ctest'.

# Copyright Daniel Brétema, 2025.
# Distributed under the Boost Software License, Version 1.0.
# See complete details at https://www.boost.org/LICENSE_1_0.txt


include(FetchContent)


###############################################################################
## Vars !

# Output Folder
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/../deps)

# Global Variable w/ lib names
set(y_meta_libs "" CACHE INTERNAL "")


###############################################################################
## Add package !

function(y_add_library lib_name lib_version lib_url allow_system)

    if (${allow_system})
        find_package(${lib_name} ${lib_version} QUIET)
    endif()


    set(get_from "System")
    if (NOT ${lib_name}_FOUND)
        set(get_from "External")
        FetchContent_Declare(${lib_name} DOWNLOAD_EXTRACT_TIMESTAMP OFF URL ${lib_url})
        FetchContent_MakeAvailable(${lib_name})
    endif()

    message(STATUS "y · ${get_from} : ${lib_name}")

    set(tmp_list "${y_meta_libs} ${lib_name}")
    string(STRIP ${tmp_list} clean_list)
    set(y_meta_libs "${clean_list}" CACHE INTERNAL "")

endfunction()


###############################################################################
## Link to project !

function(y_link_libraries project)
    string(REPLACE " " ";" local_deps "${y_meta_libs}")
    message(STATUS "y · Linking : ${project} |${local_deps}|")
    target_link_libraries(${project} PUBLIC ${local_deps})
endfunction()


###############################################################################
## Set output folder !

function(y_set_output_folder project folder_name)

    if (NOT MSVC)
        set(subfolder ${CMAKE_BUILD_TYPE})
    else()
        set(subfolder "")
    endif()

    set_target_properties(${project} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../${folder_name}/${subfolder}")
    set_target_properties(${project} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../${folder_name}/${subfolder}")
    set_target_properties(${project} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../${folder_name}/${subfolder}")

endfunction()


###############################################################################
## Glob !

function(y_glob root_dir out_sources out_headers)

    file(GLOB _sources "${root_dir}*.cpp" "${root_dir}*.cc" "${root_dir}*.c")
    set(${out_sources} "${_sources}" PARENT_SCOPE)

    file(GLOB _headers "${root_dir}*.hpp" "${root_dir}*.hh" "${root_dir}*.h")
    set(${out_headers} "${_headers}" PARENT_SCOPE)

endfunction()



###############################################################################
## Executable !

function(y_add_exe proj_name proj_root_dir)

    # Include the headers to help IDEs
    # -- https://blog.conan.io/2019/09/02/Deterministic-builds-with-C-C++.html

    y_glob(${proj_root_dir} _sources _headers)
    add_executable(${proj_name} ${_sources} ${_headers})

endfunction()


###############################################################################
## Target setup !

# function(y_target_setup proj_name output_dir do_link_libraries)

#     # Output
#     y_set_output_folder(${proj_name} "bin")

#     # Properties
#     set_target_properties(${proj_name}
#         PROPERTIES
#             CMAKE_CXX_EXTENSIONS OFF
#             CMAKE_CXX_VISIBILITY_PRESET hidden
#             CMAKE_VISIBILITY_INLINES_HIDDEN ON
#             CMAKE_EXPORT_COMPILE_COMMANDS ON
#     )

#     # Includes
#     target_include_directories(${proj_name} PUBLIC ${PROJECT_SOURCE_DIR})

#     # Dependencies
#     y_link_libraries(${proj_name})

# endfunction()


###############################################################################
## Tests !

function(y_enable_tests)

    message(STATUS "y · TESTs Enabled!")

    enable_testing()

    y_glob("${CMAKE_SOURCE_DIR}/tests/" _tests_sources _tests_headers)
    message(STATUS "y · Adding ---> ${_tests_sources}")

    foreach(test_source IN LISTS _tests_sources)

        get_filename_component(test_name "${_tests_sources}" NAME_WE)
        add_executable(${test_name} ${test_source})

        y_set_output_folder(${test_name} "tests")
        y_link_libraries(${test_name})

        add_test(NAME ${test_name} COMMAND ${test_name})

        message(STATUS "y · Added Test : ${test_name} from '${test_source}'")

    endforeach()

endfunction()
