# Jansson 2.4 CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu 

set(jansson_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(jansson_src_dir ${jansson_base_dir}/src/kurl/)
set(jansson_include_dir ${jansson_base_dir}/include)
set(jansson_lib_dir ${jansson_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${jansson_include_dir}
  ${jansson_base_dir}/extern/include/
  ${jansson_src_dir}
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${jansson_source_files}
)

set(jansson_libs
  ${jansson_lib_dir}/libjansson.a
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${jansson_libs}
)

