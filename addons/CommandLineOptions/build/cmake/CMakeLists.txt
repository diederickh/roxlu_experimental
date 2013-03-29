# Command line options cmakelists.txt

roxlu_addon_begin("command_line_options")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(cmo/CommandLineOptions.cpp)

  if(UNIX)
    roxlu_add_extern_lib(libargtable2.a)
  endif()

  if(WIN32)
    roxlu_add_extern_lib(argtable2.lib)
    roxlu_add_dll(argtable2.dll)
  endif()
  # --------------------------------------------------------------------------------------

roxlu_addon_end()

