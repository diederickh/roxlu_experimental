# icecast

roxlu_addon_begin("icecast")
  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(icecast/ShoutClient.cpp)
  
  if(WIN32)
  endif()

  if(UNIX)
    roxlu_add_extern_lib(libmp3lame.a)
    roxlu_add_extern_lib(libogg.a)
    roxlu_add_extern_lib(libshout.a)
    roxlu_add_extern_lib(libvorbis.a)
    roxlu_add_extern_lib(libtheora.a)
    roxlu_add_extern_lib(libspeex.a)
  endif()

  # --------------------------------------------------------------------------------------
roxlu_addon_end()
