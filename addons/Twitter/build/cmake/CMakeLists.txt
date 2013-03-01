# twitter 

roxlu_addon_begin("twitter")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_extern_include_dir(include/)
  roxlu_addon_add_extern_source_file(src/formats/hmac_sha1.c)
  roxlu_addon_add_extern_source_file(src/formats/json.c)
  roxlu_addon_add_extern_source_file(src/formats/Base64Encoder.cpp)

  roxlu_addon_add_source_file(twitter/OAuth.cpp)
  roxlu_addon_add_source_file(twitter/HTTP.cpp)
  roxlu_addon_add_source_file(twitter/Twitter.cpp)
  roxlu_addon_add_source_file(twitter/Buffer.cpp) 
  roxlu_addon_add_source_file(twitter/SSLBuffer.cpp)
  roxlu_addon_add_source_file(twitter/PercentEncode.cpp)


  if(APPLE) 
    find_library(fr_corefoundation CoreFoundation)
    find_library(fr_core_services CoreServices)
  
    roxlu_add_lib(${fr_corefoundation})
    roxlu_add_lib(${fr_core_services})
  
    roxlu_add_extern_lib(libcrypto.a)
    roxlu_add_extern_lib(libssl.a)
    roxlu_add_extern_lib(libuv.a)
  
  
  elseif(WIN32)
    roxlu_add_extern_lib(ssleay32.lib)
    roxlu_add_extern_lib(libeay32.lib)
    roxlu_add_extern_lib(zlib.lib)
    roxlu_add_extern_lib(libuv.lib)
  
    roxlu_add_lib(ws2_32.lib)
    roxlu_add_lib(psapi.lib)
    roxlu_add_lib(iphlpapi.lib)
  
    roxlu_add_dll(libcurl.dll)
    roxlu_add_dll(libeay32.dll)
    roxlu_add_dll(ssleay32.dll)
  
  endif()
  
  if(NOT APPLE AND UNIX)
    roxlu_add_extern_lib(libssl.a)
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libcrypto.a)
    roxlu_add_lib(dl)
    roxlu_add_lib(pthread)
    roxlu_add_lib(rt)
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()