# uv networking

roxlu_addon_begin("uv")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(uv/ClientSocket.cpp)
  roxlu_addon_add_source_file(uv/ServerSocket.cpp)
  roxlu_addon_add_source_file(uv/WorkQueue.cpp)
  roxlu_addon_add_source_file(uv/ipc/ServerIPC.cpp)
  roxlu_addon_add_source_file(uv/ipc/ClientIPC.cpp)
  roxlu_addon_add_source_file(uv/ipc/TypesIPC.cpp)
  roxlu_addon_add_source_file(uv/ipc/ParserIPC.cpp)


  if(UNIX) 
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_lib(roxlu_uv)
  endif()
  
  if(APPLE)
    find_library(fr_foundation CoreFoundation)
    find_library(fr_cs CoreServices)
    roxlu_add_lib(${fr_foundation})
    roxlu_add_lib(${fr_cs})
  endif()
  
  if(WIN32) 
    add_definitions( -DWIN32_LEAN_AND_MEAN )   # We need to do this because windows.h will include winsock.h which results in redefinitions
    roxlu_add_extern_lib(libuv.lib)
    roxlu_add_lib(ws2_32.lib)
    roxlu_add_lib(psapi.lib)
    roxlu_add_lib(iphlpapi.lib)
    roxlu_add_lib(roxlu_uv)
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
