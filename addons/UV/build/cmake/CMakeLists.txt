# UV networking
# cmake_minimum_required(VERSION 2.8)
# 
# include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake
# 
# set(uv_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
# set(uv_include_dir ${uv_base_dir}/include)
# set(uv_src_dir ${uv_base_dir}/src/uv/)
# 
# file(GLOB_RECURSE uv_source_files 
#   ${uv_src_dir}/ClientSocket.cpp
# )
# twitter 

roxlu_addon_begin("uv")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(uv/ClientSocket.cpp)
#source_group("src\\uv" FILES ${uv_source_files})

#include_directories(
#  ${uv_include_dir}
#)

if(UNIX) 
  roxlu_add_extern_lib(libuv.a)
  roxlu_add_lib(roxlu_uv)
endif()

if(WIN32) 
  add_definitions( -DWIN32_LEAN_AND_MEAN )   # We need to do this because windows.h will include winsock.h which results in redefinitions
  roxlu_add_extern_lib(libuv.lib)
  roxlu_add_lib(ws2_32.lib)
  roxlu_add_lib(psapi.lib)
  roxlu_add_lib(iphlpapi.lib)
  roxlu_add_lib(roxlu_uv)
endif()

#add_library(roxlu_uv STATIC ${uv_source_files})
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
