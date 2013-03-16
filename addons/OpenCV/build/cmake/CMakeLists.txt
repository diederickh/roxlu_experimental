# bmfont

roxlu_addon_begin("opencv")

 # --------------------------------------------------------------------------------------
 roxlu_addon_add_source_file(cv/Contour.cpp)

 roxlu_add_include_dir(${roxlu_extern_include_dir}/opencv/)
 roxlu_add_include_dir(${roxlu_extern_include_dir}/opencv2/)

 if(UNIX)
   roxlu_add_extern_lib(libopencv_stitching.a)
   roxlu_add_extern_lib(libopencv_ts.a)
   roxlu_add_extern_lib(libopencv_videostab.a)
   roxlu_add_extern_lib(libopencv_contrib.a)
   roxlu_add_extern_lib(libopencv_gpu.a)
   roxlu_add_extern_lib(libopencv_legacy.a)
   roxlu_add_extern_lib(libopencv_nonfree.a)
   roxlu_add_extern_lib(libopencv_photo.a)
   roxlu_add_extern_lib(libopencv_calib3d.a)
   roxlu_add_extern_lib(libopencv_features2d.a)
   roxlu_add_extern_lib(libopencv_ml.a)
   roxlu_add_extern_lib(libopencv_objdetect.a)
   roxlu_add_extern_lib(libopencv_video.a)
   roxlu_add_extern_lib(libopencv_core.a)
   roxlu_add_extern_lib(libopencv_flann.a)
   roxlu_add_extern_lib(libopencv_highgui.a)
   roxlu_add_extern_lib(libopencv_imgproc.a)
   roxlu_add_lib(-lz)
 endif()
   
 # --------------------------------------------------------------------------------------
roxlu_addon_end()
