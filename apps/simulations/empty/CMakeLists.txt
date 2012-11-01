cmake_minimum_required(VERSION 2.6)

# System libs, architexture, etc..
set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_OSX_ARCHITECTURES "i386")
set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -framework OpenGL -framework Cocoa -framework IOKit")
set(base ${CMAKE_CURRENT_SOURCE_DIR}/)
set(simulation_install_dir ${base}bin/)
set(CMAKE_INSTALL_PREFIX ${simulation_install_dir})

# Include directories
set(addons_base_dir ${base}../../../addons/)
set(roxlu_include_dir ${base}../../../lib/include/)
set(roxlu_lib "roxlu")
set(external_include_dir ${base}../../external/)
set(simulation_include_dir ${base}/src)
set(glfw_include_dir ${base}../../external/glfw/include/)
set(glfw_wrapper_sources ${base}../../external/glfw_wrapper/SimulationBase.cpp)
set(glfw_lib_dir ${base}../../external/glfw/lib/)

add_subdirectory(${base}/../../../lib/ ${CMAKE_CURRENT_BINARY_DIR}/roxlu)
set(simulation_libs 
  ${glfw_lib_dir}/libglfw.a
  ${roxlu_lib} 
)

# Directory name is the name of the executable
get_filename_component(simulation_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Get sources of simulation
file(GLOB_RECURSE simulation_sources src/*.cpp)
set(compile_sources 
  ${simulation_sources}
  ${glfw_wrapper_sources}
  ${roxlu_lib}
)

include_directories(
  ${glfw_include_dir} 
  ${external_include_dir}
  ${simulation_include_dir}
  ${roxlu_include_dir}
)

# Executable
add_executable(${simulation_name} ${compile_sources})
target_link_libraries(${simulation_name} ${simulation_libs})

function(add_addon name) 
  include_directories(${addons_base_dir}/${name}/include/)
  add_subdirectory(${addons_base_dir}/${name}/ ${CMAKE_CURRENT_BINARY_DIR}/addon_${name})
  target_link_libraries(${simulation_name} ${name})
  message(STATUS "Adding addon: " ${name})
endfunction(add_addon)

# ADD THE ADDONS YOU WANT TO USE IN THE SIMULATION
# ------------------------------------------------------------------
add_addon("Buttons")
# ------------------------------------------------------------------

# Installation 
install(TARGETS ${simulation_name} DESTINATION ${simulation_install_dir})

# If you want to install files too
#install(FILES ${base}/build/empty DESTINATION ${simulation_install_dir})