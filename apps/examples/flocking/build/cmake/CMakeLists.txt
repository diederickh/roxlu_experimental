# Flocking
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_add_addon("PositionBasedDynamics")
roxlu_add_addon("FLV")
roxlu_add_addon("Buttons")

roxlu_enable_opengl()

roxlu_app_initialize("flocking")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(Simulation.cpp)
   roxlu_app_add_source_file(RDebugDrawer.cpp)
   roxlu_app_add_source_file(RParticles.cpp)
   # ---------------------------------------------
roxlu_install_app()
