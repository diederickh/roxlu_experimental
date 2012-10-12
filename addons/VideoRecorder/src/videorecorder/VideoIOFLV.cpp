#include <videorecorder/VideoIOFLV.h>

int VideoIOFLV::writeOpenFileX264() {
	printf("% writeOpenFileX264()\n");
	int s =  flv.writeHeader(true, false);
	flush(flv.getBuffer());
	return s;
}

int VideoIOFLV::writeParams(VideoParams* p) {
	int s = flv.writeParams(p);
	flush(flv.getBuffer());
	return s;
}

int VideoIOFLV::writeHeaders(VideoParams* p) {
	int s = flv.writeHeaders(p);
	flush(flv.getBuffer());
	return s;
}

int VideoIOFLV::writeVideoFrame(VideoParams* p) {
	int s = flv.writeVideoFrame(p);
	flush(flv.getBuffer());
	return s;
}

// =====================================================

int VideoIOFLV::writeParamsX264(x264_param_t* p) {
//int VideoIOFLV::writeParamsX264(VideoParams* p) {
	printf("% writeParamsX264()\n");
	int s = flv.writeParamsX264(p);
	flush(flv.getBuffer());
	return s;
}

int VideoIOFLV::writeHeadersX264(x264_nal_t* nal) {
	printf("% writeHeaderX264()\n");
	int s = flv.writeHeadersX264(nal);
	flush(flv.getBuffer());
	return s;
}

/*
int VideoIOFLV::writeMetaDataX264(x264_param_t* param) {
	printf("% writeMetaDataX264()\n");
	return flv.writeMetaDataX264(param);
}
*/

int VideoIOFLV::writeFrameX264(x264_nal_t* nal, size_t size, x264_picture_t* pic) {
	printf("% writeFrameX264()\n");
	int s = flv.writeVideoFrameX264(nal, size, pic);
	flush(flv.getBuffer());
	return s;
}

int VideoIOFLV::writeCloseFile264() {
	flv.saveFile("data/particles_flv.flv");
	return 1;
}

void VideoIOFLV::flush(Buffer& buffer) {
}
