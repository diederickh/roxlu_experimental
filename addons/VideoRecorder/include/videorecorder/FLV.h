#ifndef ROXLU_VIDEORECORDER_FLVH
#define ROXLU_VIDEORECORDER_FLVH

#include <roxlu/Roxlu.h>
#include <videorecorder/FLVTypes.h>
#include <videorecorder/AMF.h>

extern "C" {
#include <x264.h>
}

class FLV {
public:
	FLV();
	~FLV();

	// reading
	int readHeader();
	int readTag(); 

	// writing
	int writeHeader(bool hasVideo = true, bool hasAudio = true);
	int writeMetaDataX264(x264_param_t* p);
	
	// io
	void loadFile(const char* filepath);
	void saveFile(const char* filepath);

	// debug
	void printType(rx_uint8 t);
private:
	Buffer buffer;
	AMF amf;
	int pos_file_size;
};
#endif




