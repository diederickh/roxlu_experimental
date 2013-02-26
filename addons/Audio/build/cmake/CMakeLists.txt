# Audio cmakelists.txt
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../lib/build/cmake/CMakeLists.txt)

set(audio_base_dir ${CMAKE_CURRENT_LIST_DIR}/../../)
set(audio_src_dir ${audio_base_dir}/src)
set(audio_include_dir ${audio_base_dir}/include)
set(audio_lib_dir ${audio_base_dir}/lib/${roxlu_platform}/${roxlu_bits})

roxlu_add_include_dir(${audio_include_dir})
roxlu_add_include_dir(${audio_src_dir})

set(audio_source_files
   ${audio_src_dir}/audio/Audio.cpp
   ${audio_src_dir}/audio/PCMWriter.cpp
)

source_group("src\\audio" FILES ${audio_source_files})

if(ROXLU_PLATFORM_WIN) 
  roxlu_add_extern_lib(portaudio_x86.lib)
  roxlu_add_dll(portaudio_x86.dll)
else() 
  roxlu_add_extern_lib(libportaudio.a)
endif(ROXLU_PLATFORM_WIN)

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

add_library(roxlu_audio STATIC ${audio_source_files})
roxlu_add_lib(roxlu_audio)

