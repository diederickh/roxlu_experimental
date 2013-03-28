# Audio recorder example: Writes raw pcm to a file which you can read into e.g.the Audacity application

cmake_minimum_required(VERSION 2.8.10)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon("Audio")

roxlu_enable_opengl()

roxlu_app_initialize("audio_recorder")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(Simulation.cpp)
   # ---------------------------------------------
roxlu_install_app()
