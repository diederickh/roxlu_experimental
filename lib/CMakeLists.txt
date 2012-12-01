# There are a couple of global variables that can be used in 
# addon or simulatin/example CMakelists.txt files:
# - roxlu_addon_sources (addons should append their sources to this list, see Twitter addon)
# - roxlu_addon_libs (addons should append their libs to this list, see Twitter addon)
# - roxlu_linker_flags (frameworks, libs, etc.. which must be linked with for your application, see simulation cmake files)

cmake_minimum_required(VERSION 2.8)

option(ROXLU_32BIT "Build 32bit version" OFF)
option(ROXLU_64BIT "Build 64bit version" ON)

set(roxlu_base_dir ${CMAKE_CURRENT_LIST_DIR}/../)
set(roxlu_src_dir ${roxlu_base_dir}lib/src/roxlu/)
set(roxlu_include_dir ${roxlu_base_dir}lib/include/)
set(roxlu_addons_dir ${roxlu_base_dir}/addons/)

# get roxlu sources
file(GLOB_RECURSE roxlu_3d_sources ${roxlu_src_dir}/3d/*.cpp)
file(GLOB_RECURSE roxlu_core_sources ${roxlu_src_dir}/core/*.cpp)
file(GLOB_RECURSE roxlu_experimental_sources ${roxlu_src_dir}/experimental/*.cpp)
file(GLOB_RECURSE roxlu_external_sources_c ${roxlu_src_dir}/external/*.c)
file(GLOB_RECURSE roxlu_external_sources_cpp ${roxlu_src_dir}/external/*.cpp)
file(GLOB_RECURSE roxlu_io_sources ${roxlu_src_dir}/io/*.cpp)
file(GLOB_RECURSE roxlu_math_sources ${roxlu_src_dir}/math/*.cpp)
file(GLOB_RECURSE roxlu_opengl_sources ${roxlu_src_dir}/opengl/*.cpp)

set(roxlu_sources
  ${roxlu_3d_sources}
  ${roxlu_core_sources}
  ${roxlu_experimental_sources}
  ${roxlu_external_sources_c}
  ${roxlu_external_sources_cpp}
  ${roxlu_io_sources}
  ${roxlu_math_sources}
  ${roxlu_opengl_sources}
)

# get roxlu include dir
set(roxlu_include_dirs 
  ${roxlu_include_dirs}
  ${roxlu_include_dir}
)

include_directories(${roxlu_include_dirs})

# global variables that can be used by addons
if(ROXLU_32BIT)
  set(roxlu_bits "32")
endif(ROXLU_32BIT)

if(ROXLU_64BIT)
  set(roxlu_bits "64")
endif(ROXLU_64BIT)

if(APPLE)
  set(roxlu_platform "mac") 
  set(ROXLU_PLATFORM_MAC TRUE)
endif(APPLE)

if(UNIX AND NOT APPLE)
  set(roxlu_platform "linux")
  set(ROXLU_PLATFORM_LINUX TRUE)
endif(UNIX AND NOT APPLE)

if(ROXLU_32BIT)
  set(CMAKE_OSX_ARCHITECTURES "i386")
endif(ROXLU_32BIT)

if(ROXLU_64BIT)
  set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif(ROXLU_64BIT)


if(ROXLU_PLATFORM_LINUX)
  set(roxlu_libs
    "pthread"
    "rt"
  )

endif(ROXLU_PLATFORM_LINUX)

# http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
add_definitions(
  "-Wno-invalid-offsetof"
  "-Wreturn-type"
  "-Werror=return-type"
)
