#ifndef ROXLU_VIDEORECORDER_VIDEOIOFLVH
#define ROXLU_VIDEORECORDER_VIDEOIOFLVH

#include <videorecorder/VideoIO.h>
#include <videorecorder/FLV.h>
#include <roxlu/Roxlu.h>

class VideoIOFLV : public VideoIO {
public:
	// X264
	int writeInitializeX264();
	int writeTagHeaderX264(x264_nal_t* nal);
	int writeMetaDataX264(x264_param_t* param);
	int writeFrameX264(rx_uint8* naldata, size_t size, x264_picture_t* pic);
	int writeShutdownX264();
private:
	FLV flv;
};

#endif



