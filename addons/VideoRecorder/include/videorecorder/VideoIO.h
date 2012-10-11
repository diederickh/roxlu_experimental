#ifndef ROXLU_VIDEORECORDER_VIDEO_IOH
#define ROXLU_VIDEORECORDER_VIDEO_IOH

#include <roxlu/Roxlu.h>

extern "C" {
#include <x264.h>
}


class VideoIO {
public:
	// X264
	virtual int writeInitializeX264() = 0;
	virtual int writeTagHeaderX264(x264_nal_t* nal) = 0;
	virtual int writeMetaDataX264(x264_param_t* param) = 0;
	virtual int writeFrameX264(rx_uint8* naldata, size_t size, x264_picture_t* pic) = 0;
	virtual int writeShutdownX264() = 0;
};

#endif




