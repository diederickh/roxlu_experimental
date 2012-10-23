#include <videorecorder/VideoIOFLV.h>


int VideoIOFLV::writeOpenFile(VideoParams* p) {
	int s = flv.writeHeader(true, true);
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

// we use writeVideoPacket(), remove this one
int VideoIOFLV::writeVideoFrame(VideoParams* p) {
	//	int s = flv.writeVideoFrame(p);
	//flush(flv.getBuffer());
	//return s;
	return -1;
}

int VideoIOFLV::writeVideoPacket(VideoPacket* pkt) {
	int s = flv.writeVideoPacket(pkt);
	flush(flv.getBuffer());
	return s;
}

int VideoIOFLV::writeAudioPacket(AudioPacket* pkt) {
	int s = flv.writeAudioPacket(pkt);
	flush(flv.getBuffer());
	return s;
}


// we use writeAudioPacket (remove this one)
int VideoIOFLV::writeAudioFrame(VideoParams* p) {
	//	int s = flv.writeAudioFrame(p);
//	flush(flv.getBuffer());
//	return s;
	return -1;
}

int VideoIOFLV::writeCloseFile(VideoParams* p) {
	flv.saveFile("data/particles_flv2.flv");
	return 0;
}


void VideoIOFLV::flush(Buffer& buffer) {
}
