#ifndef ROXLU_VIDEORECORDER_VIDEO_IOH
#define ROXLU_VIDEORECORDER_VIDEO_IOH

#include <roxlu/Roxlu.h>
#include <videorecorder/VideoTypes.h>

extern "C" {
#include <x264.h>
}

class VideoIO {
public:
	virtual int writeOpenFile(VideoParams* p) = 0;
	virtual int writeParams(VideoParams* p) = 0;
	virtual int writeHeaders(VideoParams* p) = 0;
	virtual int writeVideoFrame(VideoParams* p) = 0;
	virtual int writeAudioFrame(VideoParams* p) = 0;
	virtual int writeCloseFile(VideoParams* p) = 0;
};

#endif




