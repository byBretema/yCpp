# y.cmake - A nano wrapper over FetchContent

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
## Tests !

function(y_enable_tests)

    # message(STATUS "y · Adding Tests!")

    # enable_testing()

    # set(rel_path "${CMAKE_SOURCE_DIR}/tests/")
    # file(GLOB y_meta_tests RELATIVE "${rel_path}" "${rel_path}*.cpp" "${rel_path}*.cc" "${rel_path}*.c")
    # message(STATUS "y · Adding ---> ${y_meta_tests}")

    # foreach(test_source IN LISTS y_meta_tests)

    #     get_filename_component(test_name "${test_source}" NAME_WE)
    #     add_executable(${test_name} ${test_source})

    #     y_set_output_folder(${test_name} "tests")
    #     y_link_libraries(${test_name})

    #     add_test(NAME ${test_name} COMMAND ${test_name})

    #     message(STATUS "y · Added Test : ${test_name} from '${test_source}'")

    # endforeach()

endfunction()


###############################################################################
## How To Use !

# On parent (or project) CMake

# y_add_dep("fmt" "11.1.4" "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip")
# y_add_dep("glm" "1.0.1" "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip")
# y_add_dep("argparse" "3.2" "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip")

# On project CMake

# string(REPLACE " " ";" yList "${y_meta_libs}")
# target_link_libraries(${PROJECT_NAME} ${yList})
# ... or
# y_link_libraries(${PROJECT_NAME})

# y_enable_tests()
# This will add all the .cpp, .cc or .c in the ./tests folder as tests
# to later run with 'ctest'
