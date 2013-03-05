# external cmakelist.txt

# 1 = GLFW, 2 = OPENFRAMEWORKS, 3 = CINDER, 4 = COCOA
add_definitions("-DROXLU_GL_WRAPPER=1") 

# GLFW
set(glfw_base_dir ${CMAKE_CURRENT_LIST_DIR}/)
set(glfw_include_dir ${glfw_base_dir}/glfw/include/)
set(glfw_wrapper_sources ${glfw_base_dir}/glfw_wrapper/SimulationBase.cpp)

roxlu_add_source_file(${glfw_wrapper_sources})

source_group("src\\simulation" FILES ${glfw_wrapper_sources})

# GLEW
if(UNIX)
  roxlu_add_extern_lib(libglfw3.a)
  roxlu_add_extern_lib(libglxw.a)
#  roxlu_add_extern_lib(libGLEW.a)
#  add_definitions("-DGLEW_STATIC")
endif()

if(WIN32)
  roxlu_add_lib(opengl32.lib)
  roxlu_add_extern_lib(glew32.lib)
  roxlu_add_extern_lib(glfw3.lib)
  roxlu_add_dll(glew32.dll)
endif()

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
if(NOT APPLE AND UNIX)
  roxlu_add_lib(GL)
  roxlu_add_lib(X11)
  roxlu_add_lib(Xxf86vm)
  roxlu_add_lib(rt)
  roxlu_add_lib(/usr/lib/x86_64-linux-gnu/libXrandr.so)
endif()

roxlu_add_include_dir(${glfw_include_dir})
roxlu_add_include_dir(${CMAKE_CURRENT_LIST_DIR})
