#ifndef ROXLU_AVH 
#define ROXLU_AVH 

#undef CodecType

extern "C" {
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <time.h>
	#include <libavformat/avformat.h>
	#include <libavutil/mathematics.h>
	#include <libswscale/swscale.h>
}

struct AVContext {
	AVContext();
	
	AVFormatContext* c;
	AVOutputFormat* of;
	AVStream* vs;
	AVFrame* vframe;
	AVFrame* tmp_vframe; // used to convert RGB to desired format.
	
	SwsContext* sws;
	
	int vsize; // video output size
	uint8_t* vbuf; // video output buff
};

class AV {
public:
	AV();
	~AV();
	
	bool setup(const int srcW, const int srcH);
	bool addFrame(unsigned char* pixels);
	bool save(); // for now we write the hardcoded file.
private:
	bool setupAV();
	bool setupSWS();
	
	AVStream* addVideoStream(AVContext& context, enum CodecID codecID);
	AVFrame* allocFrame(enum PixelFormat pixFormat, int width, int height);
	bool openVideo(AVContext& avContext);
	void closeVideo(AVContext& avContext);
	
	bool can_add_frames;
	int src_w;
	int src_h;
	AVContext ct; 	
};

#endif