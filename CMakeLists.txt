cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Metadata
project(CPUPathTracer VERSION 0.1.0 LANGUAGES CXX C)

# Libraries path
include(ExternalDependencies.cmake)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

file(GLOB_RECURSE SRC_LIST "src/*.cpp")

add_library(glad "src/glad.c")
target_include_directories("glad" PRIVATE "include")

add_executable(CPUPathTracer ${SRC_LIST})

target_include_directories(CPUPathTracer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

target_link_libraries(CPUPathTracer LogiSceneGraph CppGLFW glad)

# Copy resources directory.
add_custom_command(TARGET CPUPathTracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
        )
