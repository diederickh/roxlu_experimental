# FLV recorder
# shows how to record FLV with video/audio.

cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_enable_opengl()

roxlu_add_addon(FLV)
roxlu_add_addon(VideoCapture)
roxlu_add_addon(Audio)

roxlu_app_initialize("flv_recorder")
roxlu_app_add_source_file(main.cpp)
roxlu_app_add_source_file(Simulation.cpp)

roxlu_install_app()
