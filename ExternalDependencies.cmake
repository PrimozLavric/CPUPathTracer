cmake_minimum_required(VERSION 3.6)

# Include logi scene graph
if (NOT TARGET LogiSceneGraph)
    add_subdirectory(libs/LogiSceneGraph)
else ()
    message(STATUS "[CPUPathTracer] Target logi_scene_graph is already defined. Using existing target.")
endif ()

if (NOT TARGET CppGLFW)
    add_subdirectory(libs/CppGLFW)
else ()
    message(STATUS "[CPUPathTracer] Target logi_scene_graph is already defined. Using existing target.")
endif ()