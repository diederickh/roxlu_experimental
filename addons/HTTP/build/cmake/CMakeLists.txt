# HTTP, Crypto (base64, HMAC, url-percent encode (rfc3986)

# dependencies
roxlu_add_addon(UV)

roxlu_addon_begin("http")

  # --------------------------------------------------------------------------------------
  # http client stack
  roxlu_addon_add_source_file(http/HTTP.cpp)
  roxlu_addon_add_source_file(http/HTTPConnection.cpp)
  roxlu_addon_add_source_file(http/HTTPHeader.cpp)
  roxlu_addon_add_source_file(http/HTTPHeaders.cpp)
  roxlu_addon_add_source_file(http/HTTPParameter.cpp)
  roxlu_addon_add_source_file(http/HTTPParameters.cpp)
  roxlu_addon_add_source_file(http/HTTPRequest.cpp)
  roxlu_addon_add_source_file(http/HTTPURL.cpp)
  roxlu_addon_add_source_file(http/HTTPBuffer.cpp)
  roxlu_addon_add_source_file(http/HTTPTypes.cpp)
  roxlu_addon_add_source_file(http/http_parser.c)

  # basic crypto
  roxlu_addon_add_source_file(crypto/Base64Encoder.cpp)
  roxlu_addon_add_source_file(crypto/HMAC_SHA1.c)
  roxlu_addon_add_source_file(crypto/PercentEncode.cpp)

  # oauth
  roxlu_addon_add_source_file(oauth/OAuth.cpp)

  # SSL mem bio
  roxlu_addon_add_source_file(ssl/SSLBuffer.cpp)
  roxlu_addon_add_source_file(ssl/SSLContext.cpp)

  if(APPLE)
    find_library(fr_corefoundation CoreFoundation)
    find_library(fr_core_services CoreServices)
    roxlu_add_lib(${fr_corefoundation})
    roxlu_add_lib(${fr_core_services})
    
    roxlu_add_extern_lib(libcrypto.a)
    roxlu_add_extern_lib(libssl.a)
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libjansson.a)
  endif()

  if(WIN32)

    add_definitions( -DWIN32_LEAN_AND_MEAN ) 

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

