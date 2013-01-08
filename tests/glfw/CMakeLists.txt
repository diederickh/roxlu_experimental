cmake_minimum_required(VERSION 2.6)
set(base ${CMAKE_CURRENT_SOURCE_DIR}/)
set(glfw_include_dir ${base}/include/)
set(glfw_lib ${base}libglfw.a)
 
add_subdirectory(${base}../../lib/ ${CMAKE_CURRENT_BINARY_DIR}/roxlu)
include_directories(${glfw_include_dir})
message(${glfw_include_dir})
