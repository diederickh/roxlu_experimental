cmake_minimum_required(VERSION 2.8)
option(ROXLU_32BIT "Build 32bit version" ON)

set(roxlu_base_dir ${CMAKE_CURRENT_LIST_DIR}/../)
set(roxlu_src_dir ${roxlu_base_dir}lib/src/roxlu/)
set(roxlu_include_dir ${roxlu_base_dir}lib/include/)
set(roxlu_addons_dir ${roxlu_base_dir}/addons/)

# get roxlu sources
file(GLOB_RECURSE roxlu_3d_sources ${roxlu_src_dir}/3d/*.cpp)
file(GLOB_RECURSE roxlu_core_sources ${roxlu_src_dir}/core/*.cpp)
file(GLOB_RECURSE roxlu_experimental_sources ${roxlu_src_dir}/experimental/*.cpp)
file(GLOB_RECURSE roxlu_external_sources ${roxlu_src_dir}/external/*.c)
file(GLOB_RECURSE roxlu_io_sources ${roxlu_src_dir}/io/*.cpp)
file(GLOB_RECURSE roxlu_math_sources ${roxlu_src_dir}/math/*.cpp)
file(GLOB_RECURSE roxlu_opengl_sources ${roxlu_src_dir}/opengl/*.cpp)

set(roxlu_source_files 
  ${roxlu_3d_sources}
  ${roxlu_core_sources}
  ${roxlu_experimental_sources}
  ${roxlu_external_sources}
  ${roxlu_io_sources}
  ${roxlu_math_sources}
  ${roxlu_opengl_sources}
)

# get roxlu include dir
set(roxlu_include_dirs 
  ${roxlu_include_dir}
)

include_directories(${roxlu_include_dirs})

# make static library
# add_library(roxlu STATIC ${roxlu_source_files})

# global variables that can be used by addons
if(ROXLU_32BIT)
  set(roxlu_bits "32")
endif(ROXLU_32BIT)

if(${APPLE})
  set(roxlu_platform "mac") 
endif(${APPLE})

if(ROXLU_32BIT)
  set(CMAKE_OSX_ARCHITECTURES "i386")
endif(ROXLU_32BIT)
