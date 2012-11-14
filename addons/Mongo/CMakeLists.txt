# Mongo c-driver CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../lib/CMakeLists.txt) # roxlu 

set(mongo_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(mongo_src_dir ${mongo_base_dir}/src/kurl/)
set(mongo_include_dir ${mongo_base_dir}/include)
set(mongo_lib_dir ${mongo_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

include_directories(
  ${mongo_include_dir}
  ${mongo_base_dir}/extern/include/
  ${mongo_src_dir}
)

set(roxlu_addon_sources 
  ${roxlu_addon_sources} 
  ${mongo_source_files}
)

set(mongo_libs
  ${mongo_lib_dir}/libbson.a
  ${mongo_lib_dir}/libmongoc.a
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  ${mongo_libs}
)

