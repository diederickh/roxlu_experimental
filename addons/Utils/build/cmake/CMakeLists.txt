# Utils

roxlu_addon_begin("utils")

  # --------------------------------------------------------------------------------------

  if(WIN32)
    roxlu_addon_add_source_file(utils/WindowsService.cpp)
  endif()

  roxlu_addon_add_source_file(utils/SignalHandler.cpp)
  # --------------------------------------------------------------------------------------

roxlu_addon_end()
