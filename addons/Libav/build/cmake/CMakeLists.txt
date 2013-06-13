# libav

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

    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libvpx.a)
    
    find_library(fr_cf CoreFoundation)
    find_library(fr_acc VideoDecodeAcceleration)
    roxlu_add_lib(${fr_cf})
    roxlu_add_lib(${fr_acc})
    roxlu_add_lib(-lbz2)
    roxlu_add_lib(-lz)
    roxlu_add_lib(-lm)
  endif()
  
  
  if(UNIX AND NOT APPLE)
    add_definitions(-D__STDC_CONSTANT_MACROS)

    roxlu_add_extern_lib(libavformat.a)
    roxlu_add_extern_lib(libavfilter.a)
    roxlu_add_extern_lib(libavcodec.a)
    roxlu_add_extern_lib(libavresample.a)
    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libavutil.a)
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
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libvpx.a)
    roxlu_add_extern_lib(libmp3lame.a)
    roxlu_add_lib(z)
    roxlu_add_lib(bz2)
  endif()
  
  if(WIN32) 
    roxlu_add_extern_lib(avcodec.lib)
    roxlu_add_extern_lib(avfilter.lib)
    roxlu_add_extern_lib(avformat.lib)
    roxlu_add_extern_lib(avresample.lib)
    roxlu_add_extern_lib(avutil.lib)
    roxlu_add_extern_lib(swscale.lib)

    roxlu_add_dll(avcodec-55.dll)
    roxlu_add_dll(avfilter-3.dll)
    roxlu_add_dll(avformat-55.dll)
    roxlu_add_dll(avresample-1.dll)
    roxlu_add_dll(avutil-52.dll)
    roxlu_add_dll(swscale-2.dll)

    add_definitions(-D__STDC_CONSTANT_MACROS)

  endif()
  
  roxlu_addon_add_source_file(av/AVDecoder.cpp)
  roxlu_addon_add_source_file(av/AVEncoder.cpp)
  roxlu_addon_add_source_file(av/AVEncoderThreaded.cpp)
  roxlu_addon_add_source_file(av/AVPlayer.cpp)
  roxlu_addon_add_source_file(av/AVTypes.cpp)
  roxlu_addon_add_source_file(av/AVUtils.cpp)

  # --------------------------------------------------------------------------------------

roxlu_addon_end()

roxlu_add_addon(UV)  # we use libuv for threading
roxlu_add_addon(VideoCapture) # we use the VideoCaptureGLSurface to draw decode video
roxlu_add_addon(Audio) # for audio input that we use to encode
