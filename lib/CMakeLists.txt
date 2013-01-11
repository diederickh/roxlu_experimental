# There are a couple of global variables that can be used in 
# addon or simulatin/example CMakelists.txt files:
# - roxlu_addon_sources (addons should append their sources to this list, see Twitter addon)
# - roxlu_addon_libs (addons should append their libs to this list, see Twitter addon)
# - CMAKE_EXE_LINKER_FLAG (add extra frameworks, libs etc.. to the linker flags, see Videorecorder/Audio addons)
# - for windows builds you can use ${roxlu_win_runtime} which is set to MT, MTd, MD, MDd (MD is used for import libs)
#
#
#
# General variables
# -----------------
# - roxlu_platform_lib_dir:  Used to create a per platform path for libraries.
#                            "vs2012/" "vs2010/" (including slash)
# - roxlu_platform:          String like "linux", "win", "mac"
# - roxlu_install_files:     Files you want to move to the install directory
#                            for example with ROXLU_WIN_LIB_DYNAMIC is set to TRUE
#                            you might want to copy the DLL files to the install dir.
# - roxlu_addon_libs:        If you need to link with additional libraries you can 
#                            add them to this collection.
# - roxlu_extern_lib_dir:    path to the "extern/lib/[OS]/[platform/compiler]/[arch]/ 
#                            so you can directly use something like:
#                            ${roxlu_extern_lib_dir}/libportaudio.a to target libraries.
# - roxlu_win_runtime:       For windows builds this is set to MT, MD so you know 
#                            what libraries to link against. Because we're not building
#                            all debug libraries for windows, we have a separate setting
#                            that you can use:
# - roxlu_win_runtime_project: This is set to MDd for a debug build, you can use this to
#                              include debug libs on windows.

cmake_minimum_required(VERSION 2.8)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

option(ROXLU_32BIT "Build 32bit version" OFF)
option(ROXLU_64BIT "Build 64bit version" ON)

set(roxlu_base_dir ${CMAKE_CURRENT_LIST_DIR}/../)
set(roxlu_src_dir ${roxlu_base_dir}lib/src/roxlu/)
set(roxlu_include_dir ${roxlu_base_dir}lib/include/)
set(roxlu_addons_dir ${roxlu_base_dir}/addons/)
set(roxlu_extern_src_dir ${roxlu_base_dir}/extern/src/)
set(roxlu_extern_lib_dir ${roxlu_base_dir}/extern/lib)
set(roxlu_extern_include_dir ${roxlu_base_dir}/extern/include)

# get roxlu sources
file(GLOB_RECURSE roxlu_3d_sources ${roxlu_src_dir}/3d/*.cpp)
file(GLOB_RECURSE roxlu_core_sources ${roxlu_src_dir}/core/*.cpp)
file(GLOB_RECURSE roxlu_experimental_sources ${roxlu_src_dir}/experimental/*.cpp)
file(GLOB_RECURSE roxlu_external_sources_c ${roxlu_src_dir}/external/*.c)  # part of roxlu lib
file(GLOB_RECURSE roxlu_external_sources_cpp ${roxlu_src_dir}/external/*.cpp) # part of roxlu lib
file(GLOB_RECURSE roxlu_extern_sources_c ${roxlu_extern_src_dir}/*.c) # 3rd party, used by addons
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
  ${roxlu_extern_sources_c}
)

# get roxlu include dir
set(roxlu_include_dirs 
  ${roxlu_include_dirs}
  ${roxlu_include_dir}
  ${roxlu_extern_include_dir}
)

include_directories(${roxlu_include_dirs})

# On windows we build 32bit (msvc2012 is 32bit by default)
if(WIN32)
  set(ROXLU_32BIT ON)
  set(ROXLU_64BIT OFF)
endif(WIN32)

# global variables that can be used by addons
if(ROXLU_32BIT)
  set(roxlu_bits "32")
endif(ROXLU_32BIT)

if(ROXLU_64BIT)
  set(roxlu_bits "64")
endif(ROXLU_64BIT)

if(APPLE)
  set(roxlu_platform "mac") 
  set(roxlu_platform_lib_dir "")
  set(ROXLU_PLATFORM_MAC TRUE)
endif(APPLE)

if(WIN32) 
  set(roxlu_platform "win")
  set(roxlu_platform_lib_dir "vs2010/")

  set(ROXLU_WIN_LIB_DYNAMIC TRUE)
  set(ROXLU_WIN_LIB_STATIC FALSE)
  set(ROXLU_WIN_LIB_MT FALSE)
  set(ROXLU_WIN_LIB_MD TRUE)

  set(ROXLU_PLATFORM_WIN TRUE)
endif(WIN32)

if(UNIX AND NOT APPLE)
  set(roxlu_platform "linux")
  set(roxlu_platform_lib_dir "")
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

if(NOT ROXLU_PLATFORM_WIN)
# http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
add_definitions(
  "-Wno-invalid-offsetof"
  "-Wreturn-type"
  "-Werror=return-type"
)
endif(NOT ROXLU_PLATFORM_WIN)

set(roxlu_extern_lib_dir ${roxlu_base_dir}/extern/lib/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}/)

# - On windows we don't have inttypes.h so we add a include dir
# - We also set the runtime library for windows. This is kind of experimental 
if(ROXLU_PLATFORM_WIN)
  if(ROXLU_WIN_LIB_MT)
    set(roxlu_win_runtime "MT") 
    set(roxlu_win_runtime_project "MT")
  elseif(ROXLU_WIN_LIB_MD)
    set(roxlu_win_runtime "MD") 
    set(roxlu_win_runtime_project "MD")
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(roxlu_win_runtime_project "MDd")
    endif()

  endif()

  set(CMAKE_C_FLAGS_DEBUG "/${roxlu_win_runtime_project}")
  set(CMAKE_C_FLAGS_RELEASE "/${roxlu_win_runtime_project}")
  set(CMAKE_CXX_FLAGS_DEBUG "/${roxlu_win_runtime_project}")
  set(CMAKE_CXX_FLAGS_RELEASE "/${roxlu_win_runtime_project}")


  set(roxlu_extern_include_dir
    ${roxlu_extern_include_dir}
    ${roxlu_base_dir}/extern/include/msinttypes/
    )

  add_definitions( "/wd4995 /wd4244 /wd4305 /D_CRT_SECURE_NO_WARNINGS /W3")

endif(ROXLU_PLATFORM_WIN)
