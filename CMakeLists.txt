cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Metadata
project(CPUPathTracer VERSION 0.1.0 LANGUAGES CXX)

# Libraries path
include(ExternalDependencies.cmake)

file(GLOB_RECURSE SRC_LIST "src/*.cpp")

add_executable(CPUPathTracer ${SRC_LIST})

target_include_directories(CPUPathTracer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

target_link_libraries(CPUPathTracer LogiSceneGraph CppGLFW)

# Copy resources directory.
add_custom_command(TARGET CPUPathTracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
        )
