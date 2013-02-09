# PositionBasedDynamics cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt)

set(pbd_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(pbd_include_dir ${pbd_base_dir}/include)

include_directories(
  ${pbd_include_dir}
)

