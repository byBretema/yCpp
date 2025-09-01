cmake_minimum_required(VERSION 3.25)


###############################################################################
## Project !

project(${PName})


###############################################################################
## Build Type !

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


###############################################################################
## Target !

y_add_exe(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
y_target_setup(${PROJECT_NAME} "bin" TRUE)

# # Output
# y_set_output_folder(${PROJECT_NAME} "bin")

# # Properties
# set_target_properties(${PROJECT_NAME}
#     PROPERTIES
#         CMAKE_CXX_EXTENSIONS OFF
#         CMAKE_CXX_VISIBILITY_PRESET hidden
#         CMAKE_VISIBILITY_INLINES_HIDDEN ON
#         CMAKE_EXPORT_COMPILE_COMMANDS ON
# )

# # Includes
# target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

# # Dependencies
# y_link_libraries(${PROJECT_NAME})
