cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../lib/build/cmake/CMakeLists.txt) # roxlu cmake

roxlu_enable_opengl()

roxlu_add_addon(Libav)
roxlu_add_addon(Image)
roxlu_add_addon(VideoCapture)

roxlu_app_initialize("libav_video_recorder")
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(Simulation.cpp)


   # TEST WITH MEDIA FOUNDATION BASED GRABBER
   add_definitions(-DWINVER=0x0601)
#    roxlu_app_add_source_file(VideoCaptureMediaFoundation.cpp)
#    roxlu_app_add_source_file(VideoCaptureMediaFoundationCB.cpp)
#    roxlu_app_add_source_file(VideoCaptureDirectShow.cpp)
#    roxlu_app_add_source_file(VideoCaptureDirectShowCB.cpp)
#   roxlu_app_add_source_file(MediaFoundationDebug.cpp)
   roxlu_add_lib(mf.lib)
   roxlu_add_lib(mfplay.lib)
   roxlu_add_lib(mfplat.lib)
   roxlu_add_lib(mfuuid.lib)
   roxlu_add_lib(mfreadwrite.lib)
   roxlu_add_lib(Shlwapi.lib)
   roxlu_add_lib(strmiids.lib) 

#   roxlu_app_add_source_file(Webcam.cpp)
   # ---------------------------------------------
roxlu_install_app()
