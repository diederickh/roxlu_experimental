# buttons addon cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

set(buttons_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(buttons_include_dir ${buttons_base_dir}/include)
set(buttons_src_dir ${buttons_base_dir}/src)


file(GLOB_RECURSE buttons_source_files 
  ${buttons_src_dir}/Button.cpp
  ${buttons_src_dir}/Buttons.cpp  
  ${buttons_src_dir}/Element.cpp
  ${buttons_src_dir}/Panel.cpp
  ${buttons_src_dir}/Radio.cpp
  ${buttons_src_dir}/Slider.cpp
  ${buttons_src_dir}/Storage.cpp
  ${buttons_src_dir}/Toggle.cpp
)

set(roxlu_addon_sources
  ${roxlu_addon_sources}
  ${buttons_addon_sources}
)

set(roxlu_include_dirs 
  ${roxlu_include_dirs}
  ${buttons_include_dir}
)

include_directories(
  ${buttons_include_dir}
)

set(roxlu_addon_libs
  ${roxlu_addon_libs}
  roxlu_buttons  #important to add your lib to the rest of the include libs
)

add_library(roxlu_buttons STATIC ${buttons_source_files})

