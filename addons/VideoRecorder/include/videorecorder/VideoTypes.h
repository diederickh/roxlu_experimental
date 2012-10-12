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


struct VideoParams {
	VideoParams() 
		:x264_param(NULL)
		,x264_nal(NULL)
		,x264_pic(NULL)
		,video_width(0)
		,video_height(0)
	{
	}

	int video_codec_id;
	int video_width;
	int video_height;
	
	int audio_codec_id;

	size_t x264_frame_size;
	x264_param_t* x264_param;
	x264_nal_t* x264_nal;
	x264_picture_t* x264_pic;
};

#endif




