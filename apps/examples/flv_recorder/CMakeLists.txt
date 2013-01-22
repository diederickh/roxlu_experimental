# FLV recorder
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../lib/CMakeLists.txt) # roxlu cmake
include(${roxlu_base_dir}/apps/external/CMakeLists.txt)        # External (glfw)
include(${roxlu_addons_dir}/FLV/CMakeLists.txt)                # FLV addon
include(${roxlu_addons_dir}/VideoCapture/CMakeLists.txt)       # VideoCapture addon
include(${roxlu_addons_dir}/Audio/CMakeLists.txt)              # Audio addon

set(simulation_base ${CMAKE_CURRENT_LIST_DIR}/)
set(simulation_install_dir ${simulation_base}bin/)
set(simulation_include_dir ${simulation_base}src/)
set(simulation_install_dir_windows ${CMAKE_CURRENT_BINARY_DIR}/)

set(CMAKE_INSTALL_PREFIX ${simulation_install_dir})
set(CMAKE_EXE_LINKER_FLAGS ${roxlu_linker_flags})
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-ggdb")
  endif()
  if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUGS} /Od /ZI")
    endif()
endif()

set(simulation_libs
  ${roxlu_libs}
  ${roxlu_addon_libs}
  ${glfw_libs}
)

get_filename_component(simulation_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(simulation_sources 
  src/main.cpp
  src/Simulation.cpp
)

set(compile_sources 
  ${simulation_sources}
  ${glfw_wrapper_sources}
  ${roxlu_sources}
  ${roxlu_addon_sources}
)

include_directories(
  ${glfw_include_dir} 
  ${external_include_dir}
  ${simulation_include_dir}
  ${roxlu_include_dirs}
)

# Executable
add_executable(${simulation_name} ${compile_sources})
target_link_libraries(${simulation_name} ${simulation_libs})

# Installation 
install(TARGETS ${simulation_name} DESTINATION ${simulation_install_dir})

install(FILES ${roxlu_install_files} DESTINATION ${simulation_install_dir})

if(WIN32)
  install(FILES ${roxlu_install_files} DESTINATION ${simulation_install_dir_windows}/Release)
  install(FILES ${roxlu_install_files} DESTINATION ${simulation_install_dir_windows}/Debug)
endif()
