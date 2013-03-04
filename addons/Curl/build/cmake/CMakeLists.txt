# curl

roxlu_addon_begin("curl")
  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(curl/Kurl.cpp)

  if(WIN32)
    roxlu_add_extern_lib(libcurl.lib)
    roxlu_add_extern_lib(ssleay32.lib)
    roxlu_add_extern_lib(libeay32.lib)
    roxlu_add_extern_lib(zlib.lib)
    roxlu_add_lib(ws2_32.lib)
    roxlu_add_lib(iphlpapi.lib)
    roxlu_add_lib(psapi.lib)
  
    roxlu_add_dll(libcurl.dll)
    roxlu_add_dll(libeay32.dll)
    roxlu_add_dll(ssleay32.dll)
    roxlu_add_dll(zlib1.dll)
  
  else()
    roxlu_add_extern_lib(libcurl.a)
    roxlu_add_extern_lib(libssl.a)
    roxlu_add_extern_lib(libcrypto.a)
    roxlu_add_extern_lib(libz.a)
  endif()
  
  if(UNIX AND NOT APPLE)
    roxlu_add_lib(dl)
    roxlu_add_lib(rt)
  endif()
  
  # --------------------------------------------------------------------------------------

roxlu_addon_end()

