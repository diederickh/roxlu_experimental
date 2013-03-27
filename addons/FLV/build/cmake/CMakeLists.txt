# FLV

roxlu_addon_begin("flv")
  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(flv/AV.cpp)
  roxlu_addon_add_source_file(flv/FLV.cpp)
  roxlu_addon_add_source_file(flv/FLVScreenRecorder.cpp)


  if(WIN32)
      roxlu_add_extern_lib(libuv.lib)
      roxlu_add_extern_lib(avutil.lib)
      roxlu_add_extern_lib(swscale.lib)
      roxlu_add_extern_lib(x264.lib)
      roxlu_add_extern_lib(libmp3lame.lib)
      roxlu_add_extern_lib(libmpghip-static.lib)
  
      # for libuv/sockets
      roxlu_add_lib(ws2_32.lib)
      roxlu_add_lib(psapi.lib)
      roxlu_add_lib(iphlpapi.lib)
  
      roxlu_add_dll(avcodec-54.dll)
      roxlu_add_dll(swscale-2.dll)
      roxlu_add_dll(swscale.dll)
      roxlu_add_dll(avutil.dll)
      roxlu_add_dll(avutil-52.dll)
      roxlu_add_dll(x264.dll)
      roxlu_add_dll(libmp3lame.dll)

      add_definitions( -DWIN32_LEAN_AND_MEAN )   
  
  elseif(UNIX AND NOT APPLE)
    roxlu_add_extern_lib(libx264.a)
    roxlu_add_extern_lib(libmp3lame.a)
    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libavutil.a)
    roxlu_add_extern_lib(libuv.a)
  
  elseif(APPLE)
    roxlu_add_extern_lib(libmp3lame.a)
    roxlu_add_extern_lib(libswscale.a)
    roxlu_add_extern_lib(libavutil.a)
    roxlu_add_extern_lib(libx264.a)
    roxlu_add_extern_lib(libuv.a)
  endif()
  
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
