# Audio cmakelists.txt

roxlu_addon_begin("audio")
  # --------------------------------------------------------------------------------------
  roxlu_addon_add_source_file(audio/Audio.cpp)
  roxlu_addon_add_source_file(audio/AudioFile.cpp)
  roxlu_addon_add_source_file(audio/AudioPlayer.cpp)
  roxlu_addon_add_source_file(audio/PCMWriter.cpp)
  
  if(WIN32)
    roxlu_add_extern_lib(portaudio_x86.lib)
    roxlu_add_dll(portaudio_x86.dll)
  endif()

  if(UNIX)
    roxlu_add_extern_lib(libportaudio.a)
    roxlu_add_extern_lib(libsndfile.a)
  endif()

  if(APPLE)
    find_library(fr_audio CoreAudio)
    find_library(fr_audio_unit AudioUnit)
    find_library(fr_audio_toolbox AudioToolbox)
    find_library(fr_core_services CoreServices)

    roxlu_add_lib(${fr_audio})
    roxlu_add_lib(${fr_audio_unit})
    roxlu_add_lib(${fr_audio_toolbox})
    roxlu_add_lib(${fr_core_services})

  endif(APPLE)

  # --------------------------------------------------------------------------------------
roxlu_addon_end()

