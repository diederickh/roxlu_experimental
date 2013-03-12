# bmfont

roxlu_addon_begin("bmfont")
  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(bmfont/BMFont.cpp)
  roxlu_addon_add_source_file(bmfont/BMTypes.cpp)
  roxlu_addon_add_source_file(bmfont/BMFontLoader.cpp)
  roxlu_addon_add_source_file(bmfont/BMFontRenderer.cpp)

  roxlu_add_include_dir(${roxlu_extern_include_dir}/rapidxml/)
  # --------------------------------------------------------------------------------------
roxlu_addon_end()
