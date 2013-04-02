# Command line options cmakelists.txt


roxlu_addon_begin("log")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(log/Log.cpp)


  if(UNIX) 
    roxlu_add_extern_lib(libuv.a)
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

  endif()

  # --------------------------------------------------------------------------------------

roxlu_addon_end()

