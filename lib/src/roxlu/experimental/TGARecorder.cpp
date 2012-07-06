#include <roxlu/experimental/TGARecorder.h>

// create video: ffmpeg -i small_%04d.tga -r 60 -s 640x480 -vcodec libx264 -vpre hq -b 2000k flock.mov

TGARecorder tga_recorder;

TGARecorder::TGARecorder() 
	:frame(0)
	,is_allocated(false)
	,w(0)
	,h(0)
	,pixels(NULL)
{

}

TGARecorder::~TGARecorder() {
	if(pixels != NULL) {
		delete[] pixels;
	}
}

void TGARecorder::addFrame(const std::string& name) {
	if(!is_recording) {
		return;
	}
	
	if(!is_allocated) {
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		w = viewport[2];
		h = viewport[3];
		pixels = new unsigned char[w*h*3];
	}
	
	++frame;
	char fname[1024];
	sprintf(fname, "%s_%04d.tga", name.c_str(), frame);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	
	tga.save(roxlu::File::toDataPath(fname), pixels, w, h);
}