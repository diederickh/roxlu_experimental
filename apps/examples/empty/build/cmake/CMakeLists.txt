cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake
get_filename_component(app_path ${CMAKE_CURRENT_LIST_DIR}/../../ REALPATH)  # get the directory name
get_filename_component(app_name ${app_path} NAME) # set directory name as application name

roxlu_enable_opengl()

roxlu_app_initialize(${app_name})
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(Simulation.cpp)
   # ---------------------------------------------
roxlu_install_app()
