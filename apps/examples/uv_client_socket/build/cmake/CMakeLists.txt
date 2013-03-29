# UV client socket
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon("UV")

roxlu_enable_opengl()

roxlu_app_initialize("uv")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   # ---------------------------------------------
roxlu_install_app()
