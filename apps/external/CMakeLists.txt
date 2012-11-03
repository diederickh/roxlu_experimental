cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

set(glfw_base_dir ${CMAKE_CURRENT_LIST_DIR}/)
set(glfw_include_dir ${glfw_base_dir}/glfw/include/)
set(glfw_lib_dir ${glfw_base_dir}/glfw/lib/${roxlu_platform}/${roxlu_bits})
set(glfw_wrapper_sources ${glfw_base_dir}/glfw_wrapper/SimulationBase.cpp)
set(glfw_libs ${glfw_lib_dir}/libglfw.a)

