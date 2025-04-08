#
# LightPM.cmake - A nano wrapper over FetchContent
# Lovingly typ(o)ed @byBretema
#
# MIT License
#
# Original Repository ->  https://github.com/byBretema/LightPM
#

include(FetchContent)


###############################################################################
##### Light Package Manager !

##################################
#                                #
#        Output Folder           #
#                                #
##################################

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/../deps)

##################################
#                                #
#  Global Variable w/ lib names  #
#                                #
##################################

set(LPM_MetaList "" CACHE INTERNAL "")

##################################
#                                #
#         Add package            #
#                                #
##################################

function(LPM_AddPkg pkg_name pkg_version pkg_url)
    find_package(${pkg_name} ${pkg_version} QUIET)
    if (NOT ${pkg_name}_FOUND)
        message(STATUS "LPM · External : ${pkg_name}")
        FetchContent_Declare(${pkg_name} DOWNLOAD_EXTRACT_TIMESTAMP OFF URL ${pkg_url})
        FetchContent_MakeAvailable(${pkg_name})
    else()
        message(STATUS "LPM · System : ${pkg_name}")
    endif()
    set(tmp_list "${LPM_MetaList} ${pkg_name}")
    string(STRIP ${tmp_list} clean_list)
    set(LPM_MetaList "${clean_list}" CACHE INTERNAL "")
endfunction()

##################################
#                                #
#        Link to project         #
#                                #
##################################

function(LPM_Link project)
    string(REPLACE " " ";" local_deps "${LPM_MetaList}")
    message(STATUS "LPM · Linking : ${project} |${local_deps}|")
    target_link_libraries(${project} ${local_deps})
endfunction()


###############################################################################
##### How To Use !

##################################
#                                #
#  On parent (or project) CMake  #
#                                #
##################################

# LPM_AddPkg("fmt" "11.1.4" "https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip")
# LPM_AddPkg("glm" "1.0.1" "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip")
# LPM_AddPkg("argparse" "3.2" "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.zip")

##################################
#                                #
#       On project CMake         #
#                                #
##################################

# string(REPLACE " " ";" LPM_List "${LPM_MetaList}")
# target_link_libraries(${PROJECT_NAME} ${LPM_List})
# ... or
# LPM_Link(${PROJECT_NAME})
