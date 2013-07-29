# PCRE

roxlu_addon_begin("pcre")
  # --------------------------------------------------------------------------------------
  if(UNIX)
    roxlu_add_extern_lib(libpcre.a)
    roxlu_add_extern_lib(libpcrecpp.a)
  endif()
  
  if(WIN32) 
    roxlu_add_extern_lib(pcre.lib)
    roxlu_add_extern_lib(pcrecpp.lib)
    roxlu_add_dll(pcre.dll)
    roxlu_add_dll(pcrecpp.dll)
  endif()

roxlu_addon_end()
  

