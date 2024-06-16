cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE editor_sources *.h *.cpp)
add_executable (Logi_Editor ${editor_sources})

target_include_directories(Logi_Editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS Logi_Editor DESTINATION bin)

target_link_libraries(Logi_Editor PRIVATE Logi_Engine)
