cmake_minimum_required(VERSION 3.10)

project(LogiCraft)

add_subdirectory(Code/Engine/)
add_subdirectory(Code/Editor/)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Logi_Editor)
