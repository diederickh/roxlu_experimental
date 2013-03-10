# Empty example, which compiles some addons too
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../roxlu/lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_enable_opengl()

roxlu_app_initialize("empty")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(Simulation.cpp)
   # ---------------------------------------------
roxlu_install_app()
