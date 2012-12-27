# icecast cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt)

if(ROXLU_PLATFORM_MAC)
set(roxlu_linker_flags 
   ${roxlu_linker_flags} 
)	    
endif(ROXLU_PLATFORM_MAC)

set(icecast_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(icecast_src_dir ${icecast_base_dir}/src)
set(icecast_include_dir ${icecast_base_dir}/include)

set(icecast_source_files
  ${icecast_src_dir}/icecast/ShoutClient.cpp
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${icecast_source_files}
)

set(icecast_libs
  ${roxlu_extern_lib_dir}/libmp3lame.a
  ${roxlu_extern_lib_dir}/libogg.a
  ${roxlu_extern_lib_dir}/libshout.a
  ${roxlu_extern_lib_dir}/libvorbis.a
  ${roxlu_extern_lib_dir}/libtheora.a
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${icecast_libs}
)

include_directories(
  ${icecast_include_dir}
)

add_library(roxlu_icecast STATIC ${icecast_source_files})
