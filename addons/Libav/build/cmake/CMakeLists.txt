# libav

roxlu_add_addon(UV)  # we use libuv for threading
roxlu_add_addon(VideoCapture) # we use the VideoCaptureGLSurface to draw decode video
roxlu_add_addon(Audio) # for audio input that we use to encode

roxlu_addon_begin("av")

  # --------------------------------------------------------------------------------------
  if(APPLE)
    roxlu_add_extern_lib(libavcodec.a)
    roxlu_add_extern_lib(libavfilter.a)
    roxlu_add_extern_lib(libavformat.a)
    roxlu_add_extern_lib(libavresample.a)
    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libx264.a)
    roxlu_add_extern_lib(libspeex.a)
    roxlu_add_extern_lib(libvorbis.a)
    roxlu_add_extern_lib(libvorbisenc.a)
    roxlu_add_extern_lib(libtheoraenc.a)
    roxlu_add_extern_lib(libtheoradec.a)
    roxlu_add_extern_lib(libtheora.a)
    roxlu_add_extern_lib(libogg.a)
    roxlu_add_extern_lib(libssl.a)
    roxlu_add_extern_lib(libcrypto.a)
    roxlu_add_extern_lib(libavutil.a)
    roxlu_add_extern_lib(libmp3lame.a)

# find_library(fr_foundation CoreFoundation)
#    find_library(fr_services CoreServices)

  
    
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libvpx.a)
 #   roxlu_add_lib(${fr_foundation})
    
    find_library(fr_cf CoreFoundation)
    find_library(fr_acc VideoDecodeAcceleration)
    roxlu_add_lib(${fr_cf})
    roxlu_add_lib(${fr_acc})
    roxlu_add_lib(-lbz2)
    roxlu_add_lib(-lz)
    roxlu_add_lib(-lm)
  endif()
  
  if(UNIX)
  endif()
  
  if(WIN32) 
  
  endif()
  roxlu_addon_add_source_file(av/AVDecoder.cpp)
  roxlu_addon_add_source_file(av/AVEncoder.cpp)
  roxlu_addon_add_source_file(av/AVEncoderThreaded.cpp)
  roxlu_addon_add_source_file(av/AVPlayer.cpp)
  roxlu_addon_add_source_file(av/AVTypes.cpp)
  roxlu_addon_add_source_file(av/AVUtils.cpp)

  # --------------------------------------------------------------------------------------

roxlu_addon_end()
