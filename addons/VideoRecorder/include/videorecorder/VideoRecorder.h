#ifndef ROXLU_VIDEORECORDERH
#define ROXLU_VIDEORECORDERH

#define CONVERTER_YUV 0
#define CONVERTER_SWSCALE 1
//#define RGB_CONVERTER CONVERTER_YUV
#define RGB_CONVERTER CONVERTER_SWSCALE

extern "C" {
#if RGB_CONVERTER == CONVERTER_SWSCALE
#include <libswscale/swscale.h>
#else
#include <libyuv.h> 
#endif

#include <x264.h>
}

#include <videorecorder/VideoIO.h>
#include <videorecorder/VideoIOFLV.h>
#include <videorecorder/VideoIOFLVSocket.h>

class VideoRecorder {
public:
#if RGB_CONVERTER == CONVERTER_SWSCALE
	VideoRecorder(int inW, int inH, int outW, int outH, int fps);
#else
	VideoRecorder(int inW, int inH, int fps);
#endif
	~VideoRecorder();
	bool openFile(const char* filepath);
	int addFrame(unsigned char* pixels);
	void closeFile();
	void setIO(VideoIO* io);
private:
	void initEncoder();
	void setParams();
	int writeHeaders();

private:
	int out_width;
	int out_height;
	int in_width;
	int in_height;
	int in_stride;
	int fps;
	int num_frames;
	bool vfr_input; // variable frame rate

	x264_param_t params;
	x264_t* encoder;
	x264_picture_t pic_in;
	x264_picture_t pic_out;
	x264_nal_t* nals;
	int num_nals;

	// timeing
	/*
	rx_int64 last_dts;
	rx_int64 prev_dts;
	rx_int64 first_dts;
	rx_int64 largest_pts;
	rx_int64 second_largest_pts;
	double duration;
	*/

#if RGB_CONVERTER == CONVERTER_SWSCALE
	struct SwsContext* sws;
#endif

	FILE* fp;
	VideoIO* io;
};

inline void VideoRecorder::setIO(VideoIO* io) {
	this->io = io;
}
#endif




