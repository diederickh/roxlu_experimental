#ifndef ROXLU_VIDEORECORDER_FLVH
#define ROXLU_VIDEORECORDER_FLVH

#include <roxlu/Roxlu.h>
//#include <videorecorder/FLVTypes.h>
#include <videorecorder/VideoTypes.h>
#include <videorecorder/AMF.h>

extern "C" {
#include <x264.h>
}
/*

  Some references: 
  ----------------

  - Generate flv stream: http://stackoverflow.com/questions/11862666/how-to-generate-flv-stream-from-raw-h264-which-can-be-played-by-actionscript-net
  - Describing the video packet: http://livertmpjavapublisher.blogspot.nl/2011/06/h264-avcvideopacket.html 
  - Project by MP4_Maniac on #x264dev,  <MP4_maniac> yes. L-SMASH http://code.google.com/p/l-smash/ and there is  an irc channel, #L-SMASH@freenode

*/

class FLV {
public:
	FLV();
	~FLV();

	// reading
	int readHeader();
	int readTag(); 

	// writing: production
	int writeHeader(bool hasVideo = true, bool hasAudio = true);
	int writeParams(VideoParams* p);
	int writeHeaders(VideoParams* p);
	int writeVideoFrame(VideoParams* p);
	int writeAudioFrame(VideoParams* p);

	// --------------------------
	int writeVideoPacket(VideoPacket* pkt);
	int writeAudioPacket(AudioPacket* pkt);
	// --------------------------

	// io
	void loadFile(const char* filepath);
	void saveFile(const char* filepath);
	Buffer& getBuffer();

	// debug
	void printType(rx_uint8 t);
private:
	Buffer buffer;
	AMF amf;
	int pos_file_size;
	int pos_datarate;
	int pos_duration;
	rx_uint64 time_start;

	double timebase;
	bool vfr_input;
	rx_int64 prev_dts;
	rx_int64 prev_cts;
	rx_int64 delay_time;
	rx_int64 init_delta;
	rx_int64 fps_num;
	rx_int64 fps_den;
	rx_int64 framenum;
	int delay_frames;
	bool dts_compress;

	rx_uint8* sei; // sei is copied and write is deferred after first frame
	int sei_len;
};

inline Buffer& FLV::getBuffer() {
	return buffer;
}

#endif




