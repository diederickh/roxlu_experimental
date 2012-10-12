#ifndef ROXLU_VIDEORECORDER_VIDEOIOFLVH
#define ROXLU_VIDEORECORDER_VIDEOIOFLVH

#include <videorecorder/VideoIO.h>
#include <videorecorder/FLV.h>
#include <roxlu/Roxlu.h>

class VideoIOFLV : public VideoIO {
public:
	// X264
	int writeOpenFileX264();
	int writeParamsX264(x264_param_t* p);
	int writeHeadersX264(x264_nal_t* nal);
	int writeFrameX264(x264_nal_t* nal, size_t size, x264_picture_t* pic);
	int writeCloseFile264();
	virtual void flush(Buffer& buffer); // for VideoIOFLVSocket
private:
	FLV flv;
};

#endif



