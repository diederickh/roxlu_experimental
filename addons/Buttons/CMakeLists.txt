cmake_minimum_required(VERSION 2.6)

# roxlu dirs
set(roxlu_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/)
message(${roxlu_base_dir})
set(roxlu_src_dir ${roxlu_base_dir}src/roxlu/)
set(roxlu_include_dir ${roxlu_base_dir}include/)

# addon dirs
set(addon_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(addon_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/})

# get addon sources
file(GLOB_RECURSE addon_sources ${addon_src_dir}/*.cpp)

set(addon_source_files 
  ${addon_sources}
)

# get roxlu include dir
set(roxlu_include_dirs 
  ${roxlu_include_dir}
)

include_directories(
  ${roxlu_include_dirs}
  ${addon_include_dir}
)

get_filename_component(addon_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# make static library
add_library(${addon_name} STATIC ${addon_source_files})

