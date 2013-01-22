cmake_minimum_required(VERSION 2.8)

get_filename_component(example_name ${CMAKE_CURRENT_LIST_DIR} NAME)

include(${CMAKE_CURRENT_LIST_DIR}/../../../lib/CMakeLists.txt) # roxlu cmake
include(${roxlu_addons_dir}/Twitter/CMakeLists.txt) # Twitter addon

set(CMAKE_EXE_LINKER_FLAGS ${roxlu_linker_flags})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
    endif()
endif()

file(GLOB_RECURSE example_sources src/*.cpp)

set(compile_sources
  ${example_sources}
  ${roxlu_addon_sources}
  ${roxlu_sources}
)

set(compile_libs
  ${roxlu_addon_libs}
)

set(example_base_dir ${CMAKE_CURRENT_LIST_DIR})
set(example_install_dir ${example_base_dir}/bin/)

add_executable(${example_name} ${compile_sources})
target_link_libraries(${example_name} ${compile_libs})
install(TARGETS ${example_name} DESTINATION ${example_install_dir})
