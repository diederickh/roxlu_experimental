# vpx server
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Webm)
roxlu_enable_opengl()

set(simulation_base ${CMAKE_CURRENT_LIST_DIR}/../../)
set(simulation_install_dir ${simulation_base}bin/)
set(simulation_src_dir ${simulation_base}src/)
set(simulation_install_dir_windows ${CMAKE_CURRENT_BINARY_DIR}/)
set(simulation_include_dir ${simulation_base}src/)
set(CMAKE_INSTALL_PREFIX ${simulation_install_dir})

set(simulation_libs
  ${roxlu_libs}
)

set(simulation_name vpx_server)

set(simulation_sources 
  ${simulation_src_dir}/main.cpp
  ${simulation_src_dir}/Simulation.cpp
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