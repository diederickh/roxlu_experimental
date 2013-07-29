# PCRE

roxlu_addon_begin("pcre")
  # --------------------------------------------------------------------------------------
  if(UNIX)
    roxlu_add_extern_lib(libpcre.a)
    roxlu_add_extern_lib(libpcrecpp.a)
  endif()
  
  if(WIN32) 

  endif()

roxlu_addon_end()
  

