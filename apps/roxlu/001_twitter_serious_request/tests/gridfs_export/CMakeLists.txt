cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/CMakeLists.txt) # roxlu cmake
include(${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt)
include(${roxlu_addons_dir}/Jansson/CMakeLists.txt) # Jansson addon
include(${roxlu_addons_dir}/Mongo/CMakeLists.txt) # Mongo addon

file(GLOB test_sources *.cpp)

include_directories(
        ./
)

add_definitions("-DMONGO_HAVE_STDINT")

get_filename_component(test_name ${CMAKE_CURRENT_LIST_DIR} NAME)

message(STATUS ${test_name})

add_executable(${test_name} ${test_sources})

target_link_libraries(${test_name} ${mongo_libs} ${roxlu_addon_libs})

install(TARGETS ${test_name} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/bin/)
