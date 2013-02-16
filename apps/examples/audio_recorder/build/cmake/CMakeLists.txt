# Audio recorder example
# Writes raw pcm to a file which you can read into e.g.the Audacity application

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon(Buttons)
roxlu_add_addon(Audio)
roxlu_enable_opengl()

set(simulation_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(simulation_install_dir ${simulation_base_dir}bin/)
set(simulation_include_dir ${simulation_base_dir}src/)
set(CMAKE_INSTALL_PREFIX ${simulation_install_dir})

set(simulation_libs
  ${roxlu_addon_libs}
  ${glfw_libs}
)

set(simulation_name audio_recorder)

set(simulation_sources 
  ${simulation_base_dir}/src/main.cpp
  ${simulation_base_dir}/src/Simulation.cpp
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

add_executable(${simulation_name} ${compile_sources})
target_link_libraries(${simulation_name} ${simulation_libs})

install(TARGETS ${simulation_name} DESTINATION ${simulation_install_dir})
install(FILES ${roxlu_install_files} DESTINATION ${simulation_install_dir})

if(WIN32)
  install(FILES ${roxlu_install_files} DESTINATION ${simulation_install_dir_windows}/Release)
  install(FILES ${roxlu_install_files} DESTINATION ${simulation_install_dir_windows}/Debug)
endif()

