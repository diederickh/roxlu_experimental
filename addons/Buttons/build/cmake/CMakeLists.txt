# buttons 

roxlu_addon_begin("buttons")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(buttons/Button.cpp)
  roxlu_addon_add_source_file(buttons/Buttons.cpp)  
  roxlu_addon_add_source_file(buttons/Element.cpp)
  roxlu_addon_add_source_file(buttons/Panel.cpp)
  roxlu_addon_add_source_file(buttons/Radio.cpp)
  roxlu_addon_add_source_file(buttons/Slider.cpp)
  roxlu_addon_add_source_file(buttons/Storage.cpp)
  roxlu_addon_add_source_file(buttons/Toggle.cpp)
  roxlu_addon_add_source_file(buttons/Server.cpp)
  roxlu_addon_add_source_file(buttons/Client.cpp)
  
  if(UNIX) 
    roxlu_add_extern_lib(libuv.a)
    roxlu_add_extern_lib(libconfig.a)
    roxlu_add_lib(roxlu_buttons)
  endif()
  
  if(WIN32) 
    add_definitions( -DWIN32_LEAN_AND_MEAN )   # We need to do this because windows.h will include winsock.h which results in redefinitions
    roxlu_add_extern_lib(libuv.lib)
    roxlu_add_lib(ws2_32.lib)
    roxlu_add_lib(psapi.lib)
    roxlu_add_lib(iphlpapi.lib)
    roxlu_add_lib(roxlu_buttons)
    roxlu_add_extern_lib(libconfig.lib)
    roxlu_add_dll(libconfig.dll)
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
