#ifndef ROXLU_VIDEORECORDER_VIDEO_IOH
#define ROXLU_VIDEORECORDER_VIDEO_IOH

#include <roxlu/Roxlu.h>
#include <videorecorder/VideoTypes.h>

extern "C" {
#include <x264.h>
}


class VideoIO {
public:
	// Production: VideoParams contains all info we need
	virtual int writeParams(VideoParams* p) = 0;
	virtual int writeHeaders(VideoParams* p) = 0;
	virtual int writeVideoFrame(VideoParams* p) = 0;

	// X264 (while testing we use codec specific info)
	virtual int writeOpenFileX264() = 0;
	virtual int writeParamsX264(x264_param_t* p) = 0;
	virtual int writeHeadersX264(x264_nal_t* nal) = 0;
	virtual int writeFrameX264(x264_nal_t* nal, size_t size, x264_picture_t* pic) = 0;
	virtual int writeCloseFile264() = 0;

};

#endif




