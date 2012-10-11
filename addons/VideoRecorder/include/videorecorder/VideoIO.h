#ifndef ROXLU_VIDEORECORDER_VIDEO_IOH
#define ROXLU_VIDEORECORDER_VIDEO_IOH

#include <roxlu/Roxlu.h>

extern "C" {
#include <x264.h>
}


class VideoIO {
public:
	// X264
	virtual int writeOpenFileX264() = 0;
	virtual int writeParamsX264(x264_param_t* p) = 0;
	virtual int writeHeadersX264(x264_nal_t* nal) = 0;
	virtual int writeFrameX264(x264_nal_t* nal, size_t size, x264_picture_t* pic) = 0;
	virtual int writeCloseFile264() = 0;

};

#endif




