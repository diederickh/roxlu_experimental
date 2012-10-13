#ifndef ROXLU_VIDEORECORDER_VIDEOTYPES_H
#define ROXLU_VIDEORECORDER_VIDEOTYPES_H

#include <roxlu/Roxlu.h>

extern "C" {
#include <x264.h>
}


#define FLV_TAG_NONE 0x00
#define FLV_TAG_AUDIO 8
#define FLV_TAG_VIDEO 9
#define FLV_TAG_SCRIPT_DATA 18

enum FLVSoundCodec {
	 FLV_SOUNDFORMAT_LINEAR_PCM_NE = 0 // native endan
	,FLV_SOUNDFORMAT_ADPCM = 1
	,FLV_SOUNDFORMAT_MP3 = 2
	,FLV_SOUNDFORMAT_LINEAR_PCM_LE = 3 // little endian
	,FLV_SOUNDFORMAT_NELLYMOSER_16KHZ = 4
	,FLV_SOUNDFORMAT_NELLYMOSER_8KHZ = 5
	,FLV_SOUNDFORMAT_G711_A_LAW = 7
	,FLV_SOUNDFORMAT_G711_MU_LAW = 8
	,FLV_SOUNDFORMAT_RESERVED = 9
	,FLV_SOUNDFORMAT_AAC = 10
	,FLV_SOUNDFORMAT_SPEEX = 11
	,FLV_SOUNDFORMAT_MP3_8KHZ = 14
	,FLV_SOUNDFORMAT_DEVICE_SPECIFIC = 15
};

enum FLVVideoCodec {
	  FLV_VIDEOCODEC_SORENSON = 2
	 ,FLV_VIDEOCODEC_SCREEN_VIDEO = 3
	 ,FLV_VIDEOCODEC_ON2_VP6 = 4 // h263
	 ,FLV_VIDEOCODEC_ON2_VP6_ALPHA = 5
	 ,FLV_VIDEOCODEC_SCREEN_VIDEO2 = 6
	 ,FLV_VIDEOCODEC_AVC = 7 // h264
};

enum FLVAudioSampleRate {
	 FLV_SOUNDRATE_5_5KHZ = 0 
	 ,FLV_SOUNDRATE_11KHZ = 1
	 ,FLV_SOUNDRATE_22KHZ = 2
	 ,FLV_SOUNDRATE_44KHZ = 3
};

enum FLVAudioBitDepth {
	 FLV_SOUNDSIZE_8BIT = 0
	 ,FLV_SOUNDSIZE_16BIT = 1
};

enum FLVSoundType {
	  FLV_SOUNDTYPE_MONO = 0
	 ,FLV_SOUNDTYPE_STEREO = 1
};

enum AVType {
	AV_VIDEO
	,AV_AUDIO
	,AV_NONE
};

// used while muxing
struct AVInfoPacket {
	AVInfoPacket():dts(0),av_type(AV_NONE),dx(0) {}
	rx_int64 dts; // decompression timestamp
	int av_type;  // AVType
	int dx; // refers to the index of video_packets or audio_packets in Recorder
};

struct AVInfoPacketSorter {
	bool operator()(const AVInfoPacket& a, const AVInfoPacket& b) {
		return a.dts < b.dts;
	}
};

struct VideoPacket {
	VideoPacket() 
		:dts(0)
		,pts(0)
		,is_keyframe(false)
		,data(NULL)
		,data_size(0)
	{
	}
	~VideoPacket() {
		if(data) {
			printf("~ delete data\n");
			delete[] data;
		}
	}
	rx_int64 time_dts;
	rx_int64 time_pts;
	rx_int64 dts;
	rx_int64 pts;
	bool is_keyframe;
	rx_uint8* data;
	rx_uint32 data_size;
};

struct AudioPacket {
	AudioPacket()
		:dts(0)
		,data(NULL)
		,data_size(0)
	{
	}
	~AudioPacket() {
		if(data) {
			delete[] data;
		}
	}
	rx_int64 time_dts;

	rx_int64 dts;
	rx_uint8* data;
	rx_uint32 data_size;
};

struct VideoParams {
	VideoParams() 
		:x264_param(NULL)
		,x264_nal(NULL)
		,x264_pic(NULL)
		,x264_frame_size(0)
		,video_width(0)
		,video_height(0)
		,video_codec_id(0)
		,audio_codec_id(0)
		,spx_buffer(NULL)
		,spx_num_bytes(0)
		,audio_num_encoded_samples(0)
		,audio_dts(0)
		,audio_frame_size(0)
		,audio_sample_rate(0)
		,audio_timebase(0)
	{
	}

	int video_codec_id;
	int video_width;
	int video_height;
	
	int audio_codec_id;
	char* spx_buffer;
	int spx_num_bytes;
	int audio_num_encoded_samples;
	int audio_dts;
	int audio_frame_size;
	int audio_sample_rate;
	double audio_timebase;

	size_t x264_frame_size;
	x264_param_t* x264_param;
	x264_nal_t* x264_nal;
	x264_picture_t* x264_pic;
};

#endif




