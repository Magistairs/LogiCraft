cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE engine_sources *.h *.cpp)
add_library(Logi_Engine SHARED ${engine_sources})

target_compile_definitions(Logi_Engine PRIVATE LOGI_ENGINE_EXPORTS)

target_include_directories(Logi_Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS Logi_Engine
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
