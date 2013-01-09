cmake_minimum_required(VERSION 2.6)
set(base ${CMAKE_CURRENT_LIST_DIR}/)

include(${base}/../../lib/CMakeLists.txt) # roxlu cmake
#include(${base}/../../addons/Twitter/CMakeLists.txt) # Twitter addon
include(${base}/../../addons/Instagram/CMakeLists.txt) # Instagram addon

set(test_base ${CMAKE_CURRENT_LIST_DIR}/)
set(test_install_dir ${test_base}bin/)
set(test_include_dir ${test_base}src/)
set(CMAKE_INSTALL_PREFIX ${test_install_dir})

get_filename_component(test_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE test_sources src/*.cpp)

set(compile_sources 
  ${test_sources}
  ${roxlu_sources}
  ${roxlu_addon_sources}
)

set(test_libs
  ${roxlu_addon_libs}
  ${glfw_libs}
  ${roxlu_extern_lib_dir}/libjansson.a
)

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS " -framework CoreFoundation -framework OpenGL -framework Cocoa -framework IOKit -ggdb")
endif(APPLE)


# Executable
add_executable(${test_name} ${compile_sources})
target_link_libraries(${test_name} ${test_libs})

# Installation 
install(TARGETS ${test_name} DESTINATION ${test_install_dir})




