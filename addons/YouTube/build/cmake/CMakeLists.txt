# youtube api

roxlu_add_addon(UV)
roxlu_add_addon(MsgPack)
roxlu_add_addon(SQLite)
roxlu_add_addon(Curl)

roxlu_addon_begin("youtube")

  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(youtube/ClientSocket.cpp)
  roxlu_addon_add_source_file(youtube/YouTube.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeServerIPC.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeClientIPC.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeModel.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeUpload.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeUploadStart.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeRefreshToken.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeExchangeKey.cpp)
  roxlu_addon_add_source_file(youtube/YouTubeTypes.cpp)

  if(UNIX) 
    roxlu_add_extern_lib(libjansson.a)
  endif()

  # --------------------------------------------------------------------------------------

roxlu_addon_end()
