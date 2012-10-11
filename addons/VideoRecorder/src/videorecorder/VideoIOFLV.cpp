#include <videorecorder/VideoIOFLV.h>

int VideoIOFLV::writeInitializeX264() {
	printf("% writeInitializeX264()\n");
	return flv.writeHeader(true, false);
}

int VideoIOFLV::writeTagHeaderX264(x264_nal_t* nal) {
	printf("% writeTagHeaderX264()\n");
	return -1;
}

int VideoIOFLV::writeMetaDataX264(x264_param_t* param) {
	printf("% writeMetaDataX264()\n");
	return flv.writeMetaDataX264(param);
}

int VideoIOFLV::writeFrameX264(rx_uint8* naldata, size_t size, x264_picture_t* pic) {
	return -1;
}

int VideoIOFLV::writeShutdownX264() {
	flv.saveFile("data/test.flv");
	return 1;
}
