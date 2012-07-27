#ifndef ROXLU_AVH 
#define ROXLU_AVH 

#undef CodecType

// @todo: test flv screen-video encoding type which is used for screencasts
// @todo: add audio

extern "C" {
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <time.h>
	#include <libavformat/avformat.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/audio_fifo.h>
	#include <libavutil/opt.h> // for av_opt_set
	#include <libswscale/swscale.h>
}

struct AVContext {
	AVContext();
	
	AVFormatContext* c;		// app context
	AVOutputFormat* of;		// output format 

	// Video 
	AVStream* vs; 			// video stream
	AVFrame* vframe;		// video frame
	AVFrame* tmp_vframe; 	// used to convert RGB to desired format.
	SwsContext* sws;		// scaling/converting context
	int vsize; 				// video output size
	uint8_t* vbuf; 			// video output buff
	

	// Audio
	int abit_rate;				// preferred audio bitrate 
	int asample_rate;			// audio sample rate
	AVSampleFormat asample_fmt;	// audio sample fomat 
	AVAudioFifo* afifo;			// we need to buffer audio a bit (necessary for encoder)
	AVStream* as; 				// audio stream
	int16_t* abuf; 				// audio buffer  	@todo <-- not using abuf, but afifo
	int asize;					// audio frame size
	
	// Audio - testing
	int16_t* atest_samples;
	int atest_frame_size;
	int atest_time;
};

class AV {
public:
	AV();
	~AV();
	
	bool open(const int srcW, const int srcH);
	bool addVideoFrame(unsigned char* pixels);
	bool addTestAudioFrame();
	bool addAudioFrame(unsigned char* buffer, int nsamples, int nchannels);
	bool close(); // for now we write the hardcoded file.
private:
	bool setupAV();
	bool setupSWS();
	
	AVStream* addVideoStream(AVContext& context, enum CodecID codecID);
	AVFrame* allocFrame(enum PixelFormat pixFormat, int width, int height);
	bool openVideo(AVContext& context);
	void closeVideo(AVContext& context);
	
	AVStream* addAudioStream(AVContext& context, enum CodecID codecID);
	bool openAudio(AVContext& context);
	
	bool can_add_frames;
	int src_w;
	int src_h;
	AVContext ct; 	
};

#endif