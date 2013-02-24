# external cmakelist.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/build/cmake/CMakeLists.txt)

# 1 = GLFW, 2 = OPENFRAMEWORKS, 3 = CINDER, 4 = COCOA
add_definitions("-DROXLU_GL_WRAPPER=1") 

# GLFW
set(glfw_base_dir ${CMAKE_CURRENT_LIST_DIR}/)
set(glfw_include_dir ${glfw_base_dir}/glfw/include/)
set(glfw_wrapper_sources ${glfw_base_dir}/glfw_wrapper/SimulationBase.cpp)

# GLEW
if(NOT ROXLU_PLATFORM_WIN)
  roxlu_add_extern_lib(libglfw3.a)
  roxlu_add_extern_lib(libGLEW.a)
  add_definitions("-DGLEW_STATIC")
endif()

if(ROXLU_PLATFORM_WIN)
  roxlu_add_lib(opengl32.lib)
  roxlu_add_extern_lib(glew32.lib)
  roxlu_add_extern_lib(glfw3.lib)
  roxlu_add_dll(glew32.dll)
endif(ROXLU_PLATFORM_WIN)

if(APPLE)
  find_library(fr_corefoundation CoreFoundation)
  find_library(fr_cocoa Cocoa)
  find_library(fr_opengl OpenGL)
  find_library(fr_iokit IOKit)
  
  roxlu_add_lib(${fr_corefoundation})
  roxlu_add_lib(${fr_cocoa})
  roxlu_add_lib(${fr_opengl})
  roxlu_add_lib(${fr_iokit})
endif()

# Platform specifics
if(ROXLU_PLATFORM_LINUX)
  roxlu_add_lib(GL)
  roxlu_add_lib(X11)
  roxlu_add_lib(Xxf86vm)
  roxlu_add_lib(rt)
  roxlu_add_lib(/usr/lib/x86_64-linux-gnu/libXrandr.so)
#  set(glfw_libs
#    ${glfw_libs}
#    GL
#    X11
#    Xxf86vm
#    rt
#    /usr/lib/x86_64-linux-gnu/libXrandr.so
#)
endif(ROXLU_PLATFORM_LINUX)

set(roxlu_include_dirs
  ${roxlu_include_dirs}
  ${glfw_include_dir}
  ${glew_include_dir}
  ${CMAKE_CURRENT_LIST_DIR}/
)


