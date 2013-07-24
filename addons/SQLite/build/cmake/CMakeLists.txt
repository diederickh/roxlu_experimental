# sqlite

roxlu_addon_begin("sqlite")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_extern_include_dir(sqlite/)
  roxlu_addon_add_source_file(sqlite/Database.cpp)
  roxlu_addon_add_source_file(sqlite/Query.cpp)
  roxlu_addon_add_source_file(sqlite/QueryDelete.cpp)
  roxlu_addon_add_source_file(sqlite/QueryInsert.cpp)
  roxlu_addon_add_source_file(sqlite/QueryUpdate.cpp)
  roxlu_addon_add_source_file(sqlite/QueryParam.cpp)
  roxlu_addon_add_source_file(sqlite/QueryParams.cpp)
  roxlu_addon_add_source_file(sqlite/QueryResult.cpp)
  roxlu_addon_add_source_file(sqlite/QuerySelect.cpp)
  roxlu_addon_add_extern_source_file(sqlite/sqlite3.c)

  if(UNIX AND NOT APPLE)
    roxlu_add_lib(pthread)
    roxlu_add_lib(dl)
  endif()
   # --------------------------------------------------------------------------------------

roxlu_addon_end()

