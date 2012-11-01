cmake_minimum_required(VERSION 2.6)

set(base ${CMAKE_CURRENT_SOURCE_DIR}/)
add_subdirectory(${base}/../ ${CMAKE_CURRENT_BINARY_DIR}/simulations)

set(simulation_include_dir ${base}/src)
set(glfw_include_dir ${base}../include/)
set(glfw_lib_dir ${base}../)

set(simulation_libs 
  ${glfw_lib_dir}/libglfw.a
  ${roxlu} 
)
set(CMAKE_OSX_ARCHITECTURES "i386")
set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -framework OpenGL -framework Cocoa -framework IOKit")

file(GLOB_RECURSE simulation_sources src/*.cpp)

message(${base})

# directory name is the name of the executable
get_filename_component(simulation_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message("--------------------------")

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

include_directories(${glfw_include_dir} ${simulation_include_dir}/src/)

add_executable(${simulation_name} ${simulation_sources})
target_link_libraries(${simulation_name} ${simulation_libs})

