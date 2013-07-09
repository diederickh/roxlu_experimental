# OSC


roxlu_addon_begin("osc")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(osc/Receiver.cpp)

  if(UNIX) 
    roxlu_add_extern_lib(liblo.a)
  endif()

  # --------------------------------------------------------------------------------------

roxlu_addon_end()
