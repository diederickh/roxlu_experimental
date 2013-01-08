# external cmakelist.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(ROXLU_PLATFORM_MAC)
  set(roxlu_linker_flags  "${roxlu_linker_flags}  -framework CoreFoundation -framework OpenGL -framework Cocoa -framework IOKit ")
endif(ROXLU_PLATFORM_MAC)


# 1 = GLFW, 2 = OPENFRAMEWORKS, 3 = CINDER, 4 = COCOA
add_definitions("-DROXLU_GL_WRAPPER=1") 

# GLFW
set(glfw_base_dir ${CMAKE_CURRENT_LIST_DIR}/)
set(glfw_include_dir ${glfw_base_dir}/glfw/include/)
set(glfw_lib_dir ${glfw_base_dir}/glfw/lib/${roxlu_platform}/${roxlu_bits})
set(glfw_wrapper_sources ${glfw_base_dir}/glfw_wrapper/SimulationBase.cpp)

# GLEW
set(glew_base_dir ${CMAKE_CURRENT_LIST_DIR}/)
set(glew_include_dir ${glew_base_dir}/glew/include/)
set(glew_src_dir ${glew_base_dir}/glew/src/)
set(glew_lib_dir ${glew_base_dir}/glew/lib/${roxlu_platform}/${roxlu_bits})

if(NOT ROXLU_PLATFORM_WIN)
set(glfw_libs 
  ${glfw_lib_dir}/libglfw3.a
   ${glew_lib_dir}/libGLEW.a
)
endif(NOT ROXLU_PLATFORM_WIN)

if(ROXLU_PLATFORM_WIN)
  set(glfw_libs
    opengl32.lib
    ${glew_lib_dir}/glew32_${roxlu_win_runtime}.lib
    ${glfw_lib_dir}/glfw3_${roxlu_win_runtime}.lib
    )
  add_definitions("-DGLEW_STATIC")
endif(ROXLU_PLATFORM_WIN)

# Platform specifics
if(ROXLU_PLATFORM_LINUX)
set(glfw_libs
  ${glfw_libs}
  GL
  X11
  Xxf86vm
  rt
)
endif(ROXLU_PLATFORM_LINUX)

set(roxlu_include_dirs
  ${roxlu_include_dirs}
  ${glfw_include_dir}
  ${glew_include_dir}
  ${CMAKE_CURRENT_LIST_DIR}/
)


