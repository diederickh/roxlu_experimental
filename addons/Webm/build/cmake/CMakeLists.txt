# webm addon


roxlu_add_addon(VideoCapture)  # the webm addon uses the videocapture addon to playback ivf files (see readme)
roxlu_add_addon(UV) # using libuv for the threaded uv writer 

roxlu_addon_begin("webm")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(webm/VPXEncoder.cpp)
  roxlu_addon_add_source_file(webm/VPXDecoder.cpp)
  roxlu_addon_add_source_file(webm/VPXStream.cpp)
  roxlu_addon_add_source_file(webm/EBML.cpp)
  roxlu_addon_add_source_file(webm/Webm.cpp)
  roxlu_addon_add_source_file(webm/WebmScreenRecorder.cpp)
  roxlu_addon_add_source_file(webm/IVFReader.cpp)
  roxlu_addon_add_source_file(webm/IVFWriter.cpp)
  roxlu_addon_add_source_File(webm/IVFWriterThreaded.cpp)
  roxlu_addon_add_source_file(webm/IVFPlayer.cpp)

  if(APPLE)
    find_library(fr_foundation CoreFoundation)
    find_library(fr_services CoreServices)

    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libavutil.a)
    roxlu_add_extern_lib(libavcodec.a)
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libvpx.a)
    roxlu_add_lib(${fr_foundation})
    roxlu_add_lib(${fr_services})
    
  endif()

  if(UNIX AND NOT APPLE)
#    message(FATAL_ERROR "WE NEED TO RECOMPILE LIBAV")
    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libavcodec.a)
    roxlu_add_extern_lib(libavutil.a)
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libvpx.a)
    roxlu_add_lib(pthread)
  endif()

  if(WIN32)
    message(FATAL_ERROR Webm addon is not yet ported to windows)
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()


