# y.cmake - A nano wrapper over FetchContent
# Lovingly typ(o)ed @byBretema
# MIT License

include(FetchContent)

###############################################################################
## Vars !

# Output Folder
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/../deps)
# Global Variable w/ lib names
set(y_meta_list "" CACHE INTERNAL "")

###############################################################################
## Add package !

function(y_add_library lib_name lib_version lib_url allow_system)

    if (NOT ${allow_system})
        # set(find_restricted NO_DEFAULT_PATH)
        set(get_from "Deps")
    else()
        find_package(${lib_name} ${lib_version} QUIET)
        set(get_from "System")
    endif()


    if (NOT ${lib_name}_FOUND)
        message(STATUS "y · Downloading : ${lib_name}")
        FetchContent_Declare(${lib_name} DOWNLOAD_EXTRACT_TIMESTAMP OFF URL ${lib_url})
        FetchContent_MakeAvailable(${lib_name})
    else()
        message(STATUS "y · Found on ${get_from} : ${lib_name}")
    endif()

    set(tmp_list "${y_meta_list} ${lib_name}")
    string(STRIP ${tmp_list} clean_list)
    set(y_meta_list "${clean_list}" CACHE INTERNAL "")

endfunction()

###############################################################################
## Link to project

function(y_link_libraries project)
    string(REPLACE " " ";" local_deps "${y_meta_list}")
    message(STATUS "y · Linking : ${project} |${local_deps}|")
    target_link_libraries(${project} ${local_deps})
endfunction()

###############################################################################
## How To Use !

# On parent (or project) CMake

# y_add_dep("fmt" "11.1.4" "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip")
# y_add_dep("glm" "1.0.1" "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip")
# y_add_dep("argparse" "3.2" "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip")

# On project CMake

# string(REPLACE " " ";" yList "${y_meta_list}")
# target_link_libraries(${PROJECT_NAME} ${yList})
# ... or
# y_link_libraries(${PROJECT_NAME})
