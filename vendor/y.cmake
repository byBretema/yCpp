# y.cmake - A nano wrapper over FetchContent
# Lovingly typ(o)ed @byBretema
# MIT License

include(FetchContent)

###############################################################################
## Vars !

# Output Folder
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/../deps)
# Global Variable w/ lib names
set(yMetaList "" CACHE INTERNAL "")

###############################################################################
## Add package !

function(yAdd pkg_name pkg_version pkg_url force)
    if (NOT ${force})
        find_package(${pkg_name} ${pkg_version} QUIET)
    endif()
    if (NOT ${pkg_name}_FOUND)
        message(STATUS "y · External : ${pkg_name}")
        FetchContent_Declare(${pkg_name} DOWNLOAD_EXTRACT_TIMESTAMP OFF URL ${pkg_url})
        FetchContent_MakeAvailable(${pkg_name})
    else()
        message(STATUS "y · System : ${pkg_name}")
    endif()
    set(tmp_list "${yMetaList} ${pkg_name}")
    string(STRIP ${tmp_list} clean_list)
    set(yMetaList "${clean_list}" CACHE INTERNAL "")
endfunction()

###############################################################################
## Link to project

function(yLink project)
    string(REPLACE " " ";" local_deps "${yMetaList}")
    message(STATUS "y · Linking : ${project} |${local_deps}|")
    target_link_libraries(${project} ${local_deps})
endfunction()

###############################################################################
## How To Use !

# On parent (or project) CMake

# yAdd("fmt" "11.1.4" "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip")
# yAdd("glm" "1.0.1" "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip")
# yAdd("argparse" "3.2" "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip")

# On project CMake

# string(REPLACE " " ";" yList "${yMetaList}")
# target_link_libraries(${PROJECT_NAME} ${yList})
# ... or
# yLink(${PROJECT_NAME})
