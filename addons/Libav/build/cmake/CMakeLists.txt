# libav

roxlu_addon_begin("av")

  # --------------------------------------------------------------------------------------
  if(APPLE)
    roxlu_add_extern_lib(libavcodec.a)
    roxlu_add_extern_lib(libavfilter.a)
    roxlu_add_extern_lib(libavformat.a)
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

  roxlu_addon_add_source_file(av/AVEncoder.cpp)
  roxlu_addon_add_source_file(av/Types.cpp)
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
