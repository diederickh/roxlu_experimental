#ifndef ROXLU_VIDEORECORDER_VIDEOIOFLVH
#define ROXLU_VIDEORECORDER_VIDEOIOFLVH

#include <videorecorder/VideoIO.h>
#include <videorecorder/FLV.h>
#include <videorecorder/VideoTypes.h>
#include <roxlu/Roxlu.h>

class VideoIOFLV : public VideoIO {
public:
	int writeOpenFile(VideoParams* p);
	int writeParams(VideoParams* p);
	int writeHeaders(VideoParams* p);
	int writeVideoFrame(VideoParams* p);
	int writeAudioFrame(VideoParams* p);
	int writeCloseFile(VideoParams* p);

	virtual void flush(Buffer& buffer); // for VideoIOFLVSocket
private:
	FLV flv;
};

#endif



