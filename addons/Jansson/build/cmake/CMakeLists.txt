# Jansson 

roxlu_addon_begin("jansson")

  # --------------------------------------------------------------------------------------
  if(UNIX)
    roxlu_add_extern_lib(libjansson.a)
  endif()
  
  if(WIN32) 

  endif()

  roxlu_addon_add_source_file(jansson/Jansson.cpp)
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
