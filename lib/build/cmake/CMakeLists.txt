# There are a couple of global variables that can be used in 
# addon or simulatin/example CMakelists.txt files:
# - for windows builds you can use ${roxlu_win_runtime} which is set to MT, MTd, MD, MDd (MD is used for import libs)
#
#
#
# General variables
# -----------------
# - roxlu_platform_lib_dir:                           Used to create a per platform path for libraries.
#                                                     "vs2012/" "vs2010/" (including slash)
#
# - roxlu_platform:                                   String like "linux", "win", "mac"
#
# - roxlu_install_files:                              Files you want to move to the install directory
#                                                     for example with ROXLU_WIN_LIB_DYNAMIC is set to TRUE
#                                                     you might want to copy the DLL files to the install dir.
# - roxlu_libs:                                       If you need to link with additional libraries you can 
#                                                     add them to this collection.
#
# - roxlu_extern_lib_dir:                             path to the "extern/lib/[OS]/[platform/compiler]/[arch]/ 
#                                                     so you can directly use something like:
#                                                     ${roxlu_extern_lib_dir}/libportaudio.a to target libraries.
#
# - roxlu_extern_lib_debug_dir:                       point to the debug version of the libs: adds a 'd' after 
#                                                     ${roxlu_extern_lib_dir}
#
# - roxlu_extern_bin_dir:                             Mostly used on windows to get a path to the DLL files
#
# - roxlu_extern_src_dir:                             For libraries we compile from src
#
# - roxlu_win_runtime:                                For windows builds this is set to MT, MD so you know 
#                                                     what libraries to link against. Because we're not building
#                                                     all debug libraries for windows, we have a separate setting
#                                                     that you can use:
#
# - roxlu_win_runtime_project:                        This is set to MDd for a debug build, you can use this to
#                                                     include debug libs on windows.
#
#
# Macros
# ------
# roxlu_add_addon(name)                               Adds a new addon to your projects; adds source files, include
#                                                     directories and makes sure your executable is linked with all 
#                                                     the necessary libraries
#
#
cmake_minimum_required(VERSION 2.8)

if(CMAKE_BUILD_TYPE STREQUAL Debug)

  add_definitions(-DROXLU_DEBUG)

  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()

  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Z7")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEGUG} /Od /Z7")
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
  set(addon_target_name "roxlu_${addon}")
  string(TOLOWER ${addon_target_name} addon_target_name)

  if(TARGET ${addon_target_name})
  else()
      include(${roxlu_addons_dir}/${addon}/build/cmake/CMakeLists.txt) 
  endif()

endmacro(roxlu_add_addon)

macro(roxlu_add_dll dll)

  if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL Debug AND EXISTS ${roxlu_extern_bin_debug_dir}/${dll})
      list(APPEND roxlu_install_files ${roxlu_extern_bin_debug_dir}/${dll})
    else()
      list(APPEND roxlu_install_files ${roxlu_extern_bin_dir}/${dll})
    endif()
  endif()

endmacro(roxlu_add_dll)

macro(roxlu_app_iconify)
  if(APPLE)
    set(roxlu_app_icon_file "icon.icns")
  endif()
  if(WIN32) 
    set(roxlu_app_icon_file "icon.ico")
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

# add a dylib which is found in the extern directory 
macro(roxlu_add_extern_dylib lib)
  list(APPEND roxlu_dylibs ${roxlu_extern_dylib_dir}/${lib})
  list(APPEND roxlu_install_files ${roxlu_extern_dylib_dir}/${lib})
endmacro()

# add a specific dylib, lib is a full path to this dylib (this will be installed too)
macro(roxlu_add_dylib lib)
  list(APPEND roxlu_dylibs ${lib})
  list(APPEND roxlu_install_files ${lib})
endmacro()

# install_name_tool change [old_path] @executable_path/${lib} yourapp
macro(roxlu_fix_dylib executable dylib)
  list(APPEND roxlu_dylibs_to_fix_libs ${dylib})
  list(APPEND roxlu_dylibs_to_fix_executables ${executable})
endmacro()

# add a framework from the extern/lib/mac/frameworks directory (when installing we set all the correct linker settings)
macro(roxlu_add_extern_framework name)
  list(APPEND roxlu_frameworks ${name}) 
endmacro()

# pass a list of source files; make sure to wrap quotes around the list, roxlu_add_source_files("${myfiles}")
macro(roxlu_add_source_files files)
  foreach(entry ${files})
    roxlu_add_source_file(${entry})
  endforeach()
endmacro(roxlu_add_source_files)

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

  # removed the if(WIN32) with the CMAKE_RUNTIME_* paths, because xcode was generating executables in the wrong directory
  #if(WIN32)
    # make sure the exe is stored in /bin/
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${roxlu_app_install_dir}) # necessary for Visual Studio + CMake
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${roxlu_app_install_dir})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${roxlu_app_install_dir})
    #roxlu_app_add_source_file(${roxlu_app_build_dir}/icon.rc)
  #endif()

  if(APPLE)
    if(DEFINED roxlu_app_icon_file)
      add_executable(${roxlu_app_name} MACOSX_BUNDLE ${roxlu_source_files} ${roxlu_lib_source_files} ${roxlu_app_cmake_dir}/${roxlu_app_icon_file})
      set_target_properties(${roxlu_app_name} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${roxlu_cmake_dir}/osx/Info.plist") 
    else()

      # TEST WITH SHARED LIB: 
      # ----
      #add_library(roxlu SHARED ${roxlu_lib_source_files})
      #target_link_libraries(roxlu ${roxlu_dylibs})
      # ---

      add_executable(${roxlu_app_name} ${roxlu_source_files} ${roxlu_lib_source_files})
    endif()


    # EXPERIMENTAL: Copy frameworks from extern/lib/mac/frameworks, to the install dir and set the @executable paths
    # frameworks from the extern/lib/mac/frameworks dir
    # ----------------------------------------------------------------------------
    foreach(framework_name in ${roxlu_frameworks})
      set(framework_dir ${roxlu_base_dir}/extern/lib/mac/frameworks/${framework_name}.framework)

      if(IS_DIRECTORY ${framework_dir})
        set(framework_dest_dir ${roxlu_app_install_dir}/lib/)
        set(framework_file ${framework_dest_dir}${framework_name}.framework/${framework_name})
        get_filename_component(framework_file ${framework_file} ABSOLUTE)

        # copy the framework to bin/lib directory and change the @executable path of the framework which will be then used to set the search path in the executable
        file(COPY ${framework_dir} DESTINATION ${framework_dest_dir})

        # THIS WORKS AND CHANGED THE @executable_path in the generated executable, but some frameworks link with other libs inside their own dirs... which doesnt work out
        #add_custom_command(TARGET ${roxlu_app_name} PRE_LINK
        #  COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id "@executable_path/lib/${framework_name}.framework/Versions/Current/${framework_name}" "${framework_file}"
        #  COMMENT "Changing install name"
        #  )

        # make sure that we link with this copied framework
        find_library(fr_${framework_name} ${framework_name} PATHS ${framework_dest_dir})
        roxlu_add_lib(${fr_${framework_name}})

      endif()
    endforeach()
    # ----------------------------------------------------------------------------

  endif()

  if(WIN32)

    if(DEFINED roxlu_app_icon_file)
      # Create icon resource file
      include_directories(${roxlu_app_cmake_dir})
      add_custom_command(OUTPUT ${roxlu_app_cmake_dir}/icon.o COMMAND windres.exe -I${roxlu_app_cmake_dir} -I${roxlu_cmake_dir}  -i${roxlu_cmake_dir}/win/icon.rc -o ${roxlu_app_cmake_dir}/icon.o)
      list(APPEND roxlu_source_files ${roxlu_cmake_dir}/win/icon.rc)
    endif()

    add_executable(${roxlu_app_name} ${roxlu_source_files} ${roxlu_lib_source_files})
    
    if(DEFINED roxlu_app_icon_file)
      # Remove the console 
      # set_target_properties(${roxlu_app_name} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
    
  endif()

  if(UNIX AND NOT APPLE)
    add_executable(${roxlu_app_name} ${roxlu_source_files} ${roxlu_lib_source_files})
  endif()

  #list(REMOVE_DUPLICATES roxlu_libs)
  target_link_libraries(${roxlu_app_name} ${roxlu_libs} ${roxlu_dylibs})

  install(TARGETS ${roxlu_app_name} RUNTIME DESTINATION ${roxlu_app_install_dir})
  install(FILES ${roxlu_install_files} DESTINATION ${roxlu_app_install_dir})

  ## BEGIN TESTING PACKAGING
  if(PACKAGE)
    install(TARGETS ${roxlu_app_name} 
      BUNDLE DESTINATION . COMPONENT Runtime
      RUNTIME DESTINATION . COMPONENT Runtime) 
    install(FILES ${roxlu_install_files} DESTINATION .)
  endif()
  ## END PACKAGING

  if(WIN32)
    set_target_properties(${roxlu_app_name} PROPERTIES OUTPUT_NAME_DEBUG ${roxlu_app_name})

    # copy command, copy all DLLs we need
    foreach(install_item ${roxlu_install_files})
      list(GET roxlu_install_files ${install_item} roxlu_file_to_copy)
      file(TO_NATIVE_PATH ${roxlu_file_to_copy} fcopy_from)
      file(TO_NATIVE_PATH ${roxlu_app_install_dir} fcopy_to)
      ADD_CUSTOM_COMMAND( TARGET ${roxlu_app_name} POST_BUILD
        COMMAND xcopy "${fcopy_from}" "${fcopy_to}" /y /c
        COMMENT "copying dlls ."
        )
    endforeach()

#    list(LENGTH roxlu_install_files roxlu_install_files_count)
#    math(EXPR roxlu_install_files_iter ${roxlu_install_files_count}-1)
#    foreach(i RANGE ${roxlu_install_files_iter})
#      list(GET roxlu_install_files ${i} roxlu_file_to_copy)
#      
#      file(TO_NATIVE_PATH ${roxlu_file_to_copy} fcopy_from)
#      file(TO_NATIVE_PATH ${roxlu_app_install_dir} fcopy_to)
#      ADD_CUSTOM_COMMAND( TARGET ${roxlu_app_name} POST_BUILD
#        COMMAND xcopy "${fcopy_from}" "${fcopy_to}" /y /c
#        COMMENT "copying dlls ."
#        )
#    endforeach(i)



  endif()



endmacro(roxlu_install_app)


# EXPERIMENTAL: Change the dylibs dirs to the @executable_path in the executable
# you can use `roxlu_add_dylib(path_to_dylib)` to add a dylib file to your project
# sometimes you want to store the dylibs in the same directory as your executable; 
# but because of how the way dylibs work this isn't garantueed to work all the 
# time. Therefore you can use `roxlu_fix_dylib(your_executable name_of_dylib_file)` to
# change the dylib loader path to `@executable_path:name_of_dylib_file` so your 
# app will search in the executable path for the dylib.

# When you want to use this feature, make sure that you call roxlu_fix_dylibs() as
# last thing in your cmake file.
# This file contains an example of all this: https://gist.github.com/roxlu/ad84b72409416db16109
macro(roxlu_fix_dylibs)

    list(LENGTH roxlu_dylibs_to_fix_libs len1)
    math(EXPR len2 "${len1} - 1")

    foreach(dx range ${len2})
      list(GET roxlu_dylibs_to_fix_libs ${dx} dylib_to_fix)
      list(GET roxlu_dylibs_to_fix_executables ${dx} dylib_exec)


       add_custom_command(TARGET ${roxlu_app_name} POST_BUILD
         COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${dylib_to_fix} @executable_path/${dylib_to_fix} ${roxlu_app_install_dir}/${dylib_exec}
         COMMENT "Changing install name"
         )

#       add_custom_target("DYLIB_FIXES_${dylib_to_fix}_${dylib_exec}" ALL
#         COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change ${dylib_to_fix} @executable_path/${dylib_to_fix} ${roxlu_app_install_dir}/${dylib_exec}
#         COMMENT "Changing install name"
#         )

    endforeach()

endmacro()

# --------------------------------------------------------------------------------
# A D D O N 
# --------------------------------------------------------------------------------

macro(roxlu_addon_begin name)
  set(roxlu_addon_name ${name})
  set(roxlu_addon_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
  set(roxlu_addon_include_dir ${roxlu_addon_base_dir}/include/)
  set(roxlu_addon_src_dir ${roxlu_addon_base_dir}/src/)
  include_directories(${roxlu_addon_include_dir})
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
  list(LENGTH roxlu_addon_source_files roxlu_addon_source_files_count)
  if(${roxlu_addon_source_files_count} GREATER 0)
    roxlu_add_lib("roxlu_${roxlu_addon_name}")
    add_library("roxlu_${roxlu_addon_name}" STATIC ${roxlu_addon_source_files})
  endif()

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
#  ${roxlu_src_dir}/3d/Camera.cpp
#  ${roxlu_src_dir}/3d/ArcBall.cpp
#  ${roxlu_src_dir}/3d/EasyCam.cpp
)

set(roxlu_core_sources 
  ${roxlu_src_dir}/core/Utils.cpp
  ${roxlu_src_dir}/core/Log.cpp
  ${roxlu_src_dir}/core/StringUtil.cpp
)

set(roxlu_experimental_sources 
#  ${roxlu_src_dir}/experimental/BitmapFont.cpp
)

set(roxlu_io_sources
  ${roxlu_src_dir}/io/Buffer.cpp
  ${roxlu_src_dir}/io/RingBuffer.cpp
  ${roxlu_src_dir}/io/File.cpp
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
#  ${roxlu_src_dir}/opengl/FPS.cpp
#  ${roxlu_src_dir}/opengl/Shader.cpp
)

set(roxlu_lib_source_files
  ${roxlu_3d_sources}
  ${roxlu_core_sources}
  ${roxlu_experimental_sources}
  ${roxlu_external_sources_cpp}
  ${roxlu_io_sources}
  ${roxlu_math_sources}
  ${roxlu_opengl_sources}
  ${roxlu_extern_sources_c}
)

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
  set(roxlu_platform_dylib_dir "gcc/dynamic/")
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

  list(APPEND CMAKE_MODULE_PATH "${roxlu_base_dir}/lib/build/cmake/win/modules/")

endif(WIN32)

if(UNIX AND NOT APPLE)
  set(roxlu_platform "linux")
  set(roxlu_platform_lib_dir "gcc/static/")
  set(roxlu_platform_dylib_dir "gcc/dynamic/")
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
  "-Wreturn-type"
  "-Werror=return-type"
)
endif(NOT ROXLU_PLATFORM_WIN)

set(roxlu_extern_lib_dir ${roxlu_base_dir}/extern/lib/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}/)
set(roxlu_extern_dylib_dir ${roxlu_base_dir}/extern/lib/${roxlu_platform}/${roxlu_platform_dylib_dir}${roxlu_bits}/)
set(roxlu_extern_lib_debug_dir ${roxlu_base_dir}/extern/lib/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}d/)
set(roxlu_extern_bin_dir ${roxlu_base_dir}/extern/bin/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}/)
set(roxlu_extern_bin_debug_dir ${roxlu_base_dir}/extern/bin/${roxlu_platform}/${roxlu_platform_lib_dir}${roxlu_bits}d/)

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

  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /${roxlu_win_runtime_project}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /${roxlu_win_runtime_project}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /${roxlu_win_runtime_project}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /${roxlu_win_runtime_project}")
  
  include_directories(${roxlu_base_dir}/extern/include/msinttypes/)

  add_definitions( "/wd4995 /wd4244 /wd4305 /D_CRT_SECURE_NO_WARNINGS /W3")
  
  set(roxlu_include_dirs
    ${roxlu_include_dirs}
    ${roxlu_extern_lib_dir}/curl/
    ${roxlu_extern_lib_dir}/msinttypes/
    )
    

endif(ROXLU_PLATFORM_WIN)

include_directories(${roxlu_include_dirs})

