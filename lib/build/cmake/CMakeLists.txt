# There are a couple of global variables that can be used in 
# addon or simulatin/example CMakelists.txt files:
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
# - roxlu_libs:              If you need to link with additional libraries you can 
#                            add them to this collection.
# - roxlu_extern_lib_dir:    path to the "extern/lib/[OS]/[platform/compiler]/[arch]/ 
#                            so you can directly use something like:
#                            ${roxlu_extern_lib_dir}/libportaudio.a to target libraries.
# - roxlu_extern_lib_debug_dir: point to the debug version of the libs: adds a 'd' after 
#                               ${roxlu_extern_lib_dir}
# - roxlu_extern_bin_dir:    Mostly used on windows to get a path to the DLL files
# - roxlu_extern_src_dir:    For libraries we compile from src
# - roxlu_win_runtime:       For windows builds this is set to MT, MD so you know 
#                            what libraries to link against. Because we're not building
#                            all debug libraries for windows, we have a separate setting
#                            that you can use:
# - roxlu_win_runtime_project: This is set to MDd for a debug build, you can use this to
#                              include debug libs on windows.

cmake_minimum_required(VERSION 2.8)

if(CMAKE_BUILD_TYPE STREQUAL Debug)

  add_definitions(-DROXLU_DEBUG)

  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
  endif()
endif()

# -------------------------------------------------------------------
# Basic helpers to cleanup the cmakelists.txt files of projects
#
#  roxlu_add_addon: 
#  ----------------
#  Use this macro to add addons to your project
#  pass the name of the directory that contains the 
#  addon.
#     
#     examples:
#     ---------
#     roxlu_add_addon(Curl)
#     roxlu_add_addon(SQLite)
#     roxlu_add_addon(FLV)
#
#
#  roxlu_enable_opengl:
#  ----------------
#  Sets up everything you need to use the Simulation.cpp/h files
#  and can use opengl functions.
#
# -------------------------------------------------------------------

macro(roxlu_add_addon addon)
  include(${roxlu_addons_dir}/${addon}/build/cmake/CMakeLists.txt) 
endmacro(roxlu_add_addon)

macro(roxlu_add_dll dll)

  if(WIN32)
    list(APPEND roxlu_install_files ${roxlu_extern_bin_dir}/${dll})
  endif()

endmacro(roxlu_add_dll)

macro(roxlu_app_iconify)
  if(APPLE)
    set(roxlu_app_icon_file "icon.icns")
  endif()
endmacro()

macro(roxlu_add_extern_lib lib)
  if(CMAKE_BUILD_TYPE STREQUAL Debug AND EXISTS ${roxlu_extern_lib_debug_dir}/${lib})
    list(APPEND roxlu_libs ${roxlu_extern_lib_debug_dir}/${lib})
  else()
    list(APPEND roxlu_libs ${roxlu_extern_lib_dir}/${lib})
  endif()
endmacro(roxlu_add_extern_lib)

macro(roxlu_add_lib lib)
  list(APPEND roxlu_libs ${lib})
endmacro(roxlu_add_lib)

macro(roxlu_add_source_file src_file)
  list(APPEND roxlu_source_files ${src_file})
endmacro(roxlu_add_source_file)

macro(roxlu_add_include_dir dir)
  include_directories(${dir})
endmacro(roxlu_add_include_dir)

macro(roxlu_add_extern_include_dir dir)
  include_directories(${roxlu_extern_include_dir}/${dir})
endmacro()

macro(roxlu_enable_opengl3)
  add_definitions(-DROXLU_WITH_GLFW3_AND_GLXW)
  set(roxlu_gl_version 3)
  include(${roxlu_base_dir}/apps/external/CMakeLists.txt) # External (glfw)
endmacro()

macro(roxlu_enable_opengl)
  add_definitions(-DROXLU_WITH_GLFW3_AND_GLEW)
  set(roxlu_gl_version 2)
  include(${roxlu_base_dir}/apps/external/CMakeLists.txt) # External (glfw)
endmacro(roxlu_enable_opengl)

# --------------------------------------------------------------------------------
# A P P L I C A T I O N 
# --------------------------------------------------------------------------------

macro(roxlu_app_initialize app_name)

  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(roxlu_app_name ${app_name}_debug)
    message("-------------------------------------- ${roxlu_app_name} =========================")
  else()
    set(roxlu_app_name ${app_name})
  endif()

  set(roxlu_app_cmake_dir ${CMAKE_CURRENT_LIST_DIR})
  set(roxlu_app_base_dir ${CMAKE_CURRENT_LIST_DIR}/../..)
  set(roxlu_app_build_dir ${CMAKE_CURRENT_LIST_DIR}/)
  set(roxlu_app_install_dir ${roxlu_app_base_dir}/bin)
  set(roxlu_app_include_dir ${roxlu_app_base_dir}/src)
  set(roxlu_app_src_dir ${roxlu_app_base_dir}/src)
  set(CMAKE_INSTALL_PREFIX ${roxlu_app_install_dir})
  roxlu_add_include_dir(${roxlu_app_include_dir})

endmacro(roxlu_app_initialize)

macro(roxlu_app_add_source_file file)
  roxlu_add_source_file(${roxlu_app_src_dir}/${file})
  source_group("src\\simulation" FILES ${file})
endmacro(roxlu_app_add_source_file)


# function(JOIN VALUES GLUE OUTPUT)
#   string (REPLACE ";" "${GLUE}" _TMP_STR "${VALUES}")
#   set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
# endfunction()

macro(roxlu_install_app)
  if(WIN32)
    # make sure the exe is stored in /bin/
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${roxlu_app_install_dir})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${roxlu_app_install_dir})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${roxlu_app_install_dir})
    #roxlu_app_add_source_file(${roxlu_app_build_dir}/icon.rc)
  endif()

  if(APPLE)
    if(DEFINED roxlu_app_icon_file)
      add_executable(${roxlu_app_name} MACOSX_BUNDLE ${roxlu_source_files} ${roxlu_lib_source_files} ${roxlu_app_cmake_dir}/${roxlu_app_icon_file})
      set_target_properties(${roxlu_app_name} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${roxlu_cmake_dir}/osx/Info.plist") 
    else()
      add_executable(${roxlu_app_name} ${roxlu_source_files} ${roxlu_lib_source_files})
    endif()
  else()
    add_executable(${roxlu_app_name} ${roxlu_source_files} ${roxlu_lib_source_files})
  endif()
  target_link_libraries(${roxlu_app_name} ${roxlu_libs})

  install(TARGETS ${roxlu_app_name} DESTINATION ${roxlu_app_install_dir})
  install(FILES ${roxlu_install_files} DESTINATION ${roxlu_app_install_dir})

  if(WIN32)
    set_target_properties(${roxlu_app_name} PROPERTIES OUTPUT_NAME_DEBUG ${roxlu_app_name})

    # copy command, copy all DLLs we need
    list(LENGTH roxlu_install_files roxlu_install_files_count)
    math(EXPR roxlu_install_files_iter ${roxlu_install_files_count}-1)

    foreach(i RANGE ${roxlu_install_files_iter})
      list(GET roxlu_install_files ${i} roxlu_file_to_copy)
      file(TO_NATIVE_PATH ${roxlu_file_to_copy} fcopy_from)
      file(TO_NATIVE_PATH ${roxlu_app_install_dir} fcopy_to)
      ADD_CUSTOM_COMMAND( TARGET ${roxlu_app_name} POST_BUILD
        COMMAND xcopy "${fcopy_from}" "${fcopy_to}" /y /C
        COMMENT "copying dlls ."
      )
    endforeach(i)


endif()

endmacro(roxlu_install_app)


# --------------------------------------------------------------------------------
# A D D O N 
# --------------------------------------------------------------------------------

macro(roxlu_addon_begin name)
  set(roxlu_addon_name ${name})
  set(roxlu_addon_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
  set(roxlu_addon_include_dir ${roxlu_addon_base_dir}/include/)
  set(roxlu_addon_src_dir ${roxlu_addon_base_dir}/src/)
  include_directories(${roxlu_addon_include_dir})
  roxlu_add_lib("roxlu_${roxlu_addon_name}")
endmacro()

macro(roxlu_addon_add_source_file file)
  list(APPEND roxlu_addon_source_files ${roxlu_addon_src_dir}/${file})
endmacro()

macro(roxlu_addon_add_include_dir dir)
  include_directories(${roxlu_addon_include_dir}/${dir})
endmacro()

macro(roxlu_addon_add_extern_include_dir dir)
  include_directories(${roxlu_addon_base_dir}/extern/${dir})
endmacro()

macro(roxlu_addon_add_extern_source_file file)
  list(APPEND roxlu_addon_source_files ${roxlu_addon_base_dir}/extern/${file})
endmacro()

macro(roxlu_addon_add_source_files glob) 
  file(GLOB_RECURSE addon_source_files ${roxlu_addon_src_dir}/*.cpp)
  list(APPEND roxlu_addon_source_files ${addon_source_files})
endmacro()

macro(roxlu_addon_end)
  add_library("roxlu_${roxlu_addon_name}" STATIC ${roxlu_addon_source_files})

  set(roxlu_addon_source_files "")
  set(roxlu_addon_name "")
  set(roxlu_addon_base_dir "")
  set(roxlu_addon_include_dir "")
  set(roxlu_addon_src_dir "")
endmacro()

# --------------------------------------------------------------------------------
# R O X L U   
# --------------------------------------------------------------------------------

#option(ROXLU_32BIT "Build 32bit version" OFF)
#option(ROXLU_64BIT "Build 64bit version" ON)

set(roxlu_cmake_dir ${CMAKE_CURRENT_LIST_DIR})
set(roxlu_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../../)
set(roxlu_src_dir ${roxlu_base_dir}lib/src/roxlu/)
set(roxlu_include_dir ${roxlu_base_dir}lib/include/)
set(roxlu_addons_dir ${roxlu_base_dir}/addons/)
set(roxlu_extern_src_dir ${roxlu_base_dir}/extern/src/)
set(roxlu_extern_include_dir ${roxlu_base_dir}/extern/include)
set(roxlu_extern_src_dir ${roxlu_base_dir}/extern/src)

# get roxlu sources
set(roxlu_3d_sources
  ${roxlu_src_dir}/3d/Camera.cpp
  ${roxlu_src_dir}/3d/ArcBall.cpp
  ${roxlu_src_dir}/3d/EasyCam.cpp
  ${roxlu_src_dir}/3d/shapes/Axis.cpp
)

set(roxlu_core_sources 
#  ${roxlu_src_dir}/core/Thread.cpp
  ${roxlu_src_dir}/core/Utils.cpp
  ${roxlu_src_dir}/core/StringUtil.cpp
)

set(roxlu_experimental_sources 
  ${roxlu_src_dir}/experimental/Image.cpp
#  ${roxlu_src_dir}/experimental/StereoCam.cpp
  ${roxlu_src_dir}/experimental/DebugDrawer.cpp
#  ${roxlu_src_dir}/experimental/Tweens.cpp
  ${roxlu_src_dir}/experimental/BitmapFont.cpp
  ${roxlu_src_dir}/experimental/Font.cpp
#  ${roxlu_src_dir}/experimental/SimpleAnim.cpp
#  ${roxlu_src_dir}/experimental/Tween.cpp
#  ${roxlu_src_dir}/experimental/TGA.cpp
#  ${roxlu_src_dir}/experimental/TGARecorder.cpp
  ${roxlu_src_dir}/experimental/Text.cpp
)

set(roxlu_io_sources
  ${roxlu_src_dir}/io/Buffer.cpp
  ${roxlu_src_dir}/io/RingBuffer.cpp
  ${roxlu_src_dir}/io/INI.cpp
  ${roxlu_src_dir}/io/File.cpp
  ${roxlu_src_dir}/io/OBJ.cpp
)

set(roxlu_math_sources 
  ${roxlu_src_dir}/math/Quat.cpp
  ${roxlu_src_dir}/math/Mat4.cpp
  ${roxlu_src_dir}/math/Mat3.cpp
  ${roxlu_src_dir}/math/Vec2.cpp
  ${roxlu_src_dir}/math/Vec3.cpp
  ${roxlu_src_dir}/math/Vec4.cpp
  ${roxlu_src_dir}/math/Noise.cpp
)

set(roxlu_opengl_sources 
#  ${roxlu_src_dir}/opengl/FBO.cpp
#  ${roxlu_src_dir}/opengl/Texture.cpp
  ${roxlu_src_dir}/opengl/FPS.cpp
  ${roxlu_src_dir}/opengl/Shader.cpp
#  ${roxlu_src_dir}/opengl/PBO.cpp
#  ${roxlu_src_dir}/opengl/Billboard.cpp
#  ${roxlu_src_dir}/opengl/VAO.cpp
#  ${roxlu_src_dir}/opengl/MatrixStack.cpp
)

# 3rd party, used by addons
set(roxlu_extern_sources_c 
  ${roxlu_src_dir}/external/http_parser.c
  ${roxlu_src_dir}/external/stb_image_write.c
  ${roxlu_src_dir}/external/slre.c
  ${roxlu_src_dir}/external/stb_truetype.c
) 

set(roxlu_lib_source_files
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

#add_library(roxlu static ${roxlu_base_source_files})
#roxlu_add_lib(roxlu)

source_group("src\\roxlu\\3d" FILES ${roxlu_3d_sources})
source_group("src\\roxlu\\core" FILES ${roxlu_core_sources})
source_group("src\\roxlu\\experimental" FILES ${roxlu_experimental_sources})
source_group("src\\roxlu\\external" FILES ${roxlu_external_sources_c} ${roxlu_external_source_cpp})
source_group("src\\roxlu\\io" FILES ${roxlu_io_sources})
source_group("src\\roxlu\\math" FILES ${roxlu_math_sources})
source_group("src\\roxlu\\opengl" FILES ${roxlu_opengl_sources})
source_group("src\\extern" FILES ${roxlu_extern_sources_c})

# get roxlu include dir
set(roxlu_include_dirs 
  ${roxlu_include_dirs}
  ${roxlu_include_dir}
  ${roxlu_extern_include_dir}
)

if(NOT ROXLU_32BIT AND NOT ROXLU_64BIT)
  set(ROXLU_64BIT ON)
endif()

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
  set(roxlu_platform_lib_dir "gcc/static/")
  set(ROXLU_PLATFORM_MAC TRUE)
endif(APPLE)

if(WIN32) 
  set(ROXLU_WIN_LIB_MT FALSE)
  set(ROXLU_WIN_LIB_MD TRUE)

  set(roxlu_platform "win")

  if(ROXLU_WIN_LIB_MT)
    set(roxlu_platform_lib_dir "vs2010/MT/")
  else()
    set(roxlu_platform_lib_dir "vs2010/MD/")
  endif()

  set(ROXLU_PLATFORM_WIN TRUE)

endif(WIN32)

if(UNIX AND NOT APPLE)
  set(roxlu_platform "linux")
  set(roxlu_platform_lib_dir "gcc/static/")
  set(ROXLU_PLATFORM_LINUX TRUE)
endif(UNIX AND NOT APPLE)

if(ROXLU_32BIT)
  set(CMAKE_OSX_ARCHITECTURES "i386")
endif(ROXLU_32BIT)

if(ROXLU_64BIT)
  set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif(ROXLU_64BIT)


if(NOT ROXLU_PLATFORM_WIN)
# http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
add_definitions(
  "-Wno-invalid-offsetof"
  "-Wreturn-type"
  "-Werror=return-type"
)
endif(NOT ROXLU_PLATFORM_WIN)

set(roxlu_extern_lib_dir ${roxlu_base_dir}/extern/lib/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}/)
set(roxlu_extern_lib_debug_dir ${roxlu_base_dir}/extern/lib/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}d/)
set(roxlu_extern_bin_dir ${roxlu_base_dir}/extern/bin/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}/)

set(roxlu_include_dirs
  ${roxlu_include_dirs}
  ${roxlu_extern_lib_dir}/curl/ 
)


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

  
  include_directories(${roxlu_base_dir}/extern/include/msinttypes/)

  add_definitions( "/wd4995 /wd4244 /wd4305 /D_CRT_SECURE_NO_WARNINGS /W3")
  
  set(roxlu_include_dirs
    ${roxlu_include_dirs}
    ${roxlu_extern_lib_dir}/curl/
    ${roxlu_extern_lib_dir}/msinttypes/
    )
    

endif(ROXLU_PLATFORM_WIN)

include_directories(${roxlu_include_dirs})

