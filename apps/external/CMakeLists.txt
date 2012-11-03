# external cmakelist.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(APPLE)
  set(
    roxlu_linker_flags 
    ${roxlu_linker_flags} 
    " -framework CoreFoundation -framework OpenGL -framework Cocoa -framework IOKit "
    )
endif(APPLE)



add_definitions("-DROXLU_GL_WRAPPER=1")

set(glfw_base_dir ${CMAKE_CURRENT_LIST_DIR}/)
set(glfw_include_dir ${glfw_base_dir}/glfw/include/)
set(glfw_lib_dir ${glfw_base_dir}/glfw/lib/${roxlu_platform}/${roxlu_bits})
set(glfw_wrapper_sources ${glfw_base_dir}/glfw_wrapper/SimulationBase.cpp)
set(glfw_libs ${glfw_lib_dir}/libglfw3.a)

set(roxlu_include_dirs
  ${roxlu_include_dirs}
  ${glfw_include_dir}
  ${CMAKE_CURRENT_LIST_DIR}/
)


