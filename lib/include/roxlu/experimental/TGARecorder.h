#ifndef ROXLU_TGA_RECORDERH
#define ROXLU_TGA_RECORDERH

#include <roxlu/experimental/TGA.h>
#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/io/File.h>
#include <string>

class TGARecorder {
public:
	TGARecorder();
	~TGARecorder();
	void addFrame(const std::string& name);
	void toggle();
	bool is_recording;
	roxlu::TGA tga;
	int frame;
	bool is_allocated;
	
	unsigned char* pixels;
	int w; 
	int h;
};

inline void TGARecorder::toggle() {
	is_recording = !is_recording;
}

extern TGARecorder tga_recorder;

#endif