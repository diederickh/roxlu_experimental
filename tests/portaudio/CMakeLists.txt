# Example from portaudio
cmake_minimum_required(VERSION 2.6)
set(base ${CMAKE_CURRENT_LIST_DIR}/)

include(${base}/../../lib/CMakeLists.txt) # roxlu cmake
include(${base}/../../addons/Audio/CMakeLists.txt) # Audio addon

set(test_base ${CMAKE_CURRENT_LIST_DIR}/)
set(test_install_dir ${test_base}bin/)
set(test_include_dir ${test_base}src/)

set(CMAKE_INSTALL_PREFIX ${test_install_dir})
set(CMAKE_EXE_LINKER_FLAGS ${roxlu_linker_flags})

get_filename_component(test_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE test_sources_cpp src/*.cpp)
file(GLOB_RECURSE test_sources_c src/*.c)

set(compile_sources 
  ${test_sources_c}
  ${test_sources_cpp}
  ${roxlu_sources}
  ${roxlu_addon_sources}
)

set(test_libs
  ${roxlu_addon_libs}
)

# Executable
add_executable(${test_name} ${compile_sources})
target_link_libraries(${test_name} ${test_libs})

message(${test_name})
# Installation 
install(TARGETS ${test_name} DESTINATION ${test_install_dir})




