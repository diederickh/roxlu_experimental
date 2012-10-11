#include <videorecorder/FLV.h>

FLV::FLV()
	:pos_file_size(0)
{
}

FLV::~FLV() {

}

// IO
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FLV::loadFile(const char* filepath) {
	buffer.loadFile(filepath);
}


void FLV::saveFile(const char* filepath) {
	buffer.print();
	// rewrite the file size.
	Buffer tmp;
	amf.putNumberAMF0(tmp, AMFNumber(buffer.size()));
	buffer.putBytes(tmp.getPtr(), tmp.size(), pos_file_size);

	// save the buffer
	buffer.saveFile(filepath);
}


// R E A D I N G
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FLV::readHeader() {
	// should print 'FLV'
	char file_tag[4];
	buffer.getBytes(file_tag, 3);
	file_tag[3] = '\0';
	printf("File tag: %s\n", file_tag);

	// version
	rx_uint8 b8 = buffer.getByte();
	printf("Version: %d\n", b8);

	// Audio / Video flag
	b8 = buffer.getByte();
	printf("Audio/Video flag: %02x\n", b8);

	// Data offset
	rx_uint32 b32 = buffer.getBigEndianU32();
	printf("Data offset: %d\n", b32);

	// Previous tag size
	b32 = buffer.getBigEndianU32();
	printf("Previous tag size: %d\n", b32);
	return -1;
}

int FLV::readTag() {
	printf("++++++++\n");
	rx_uint8 tag_id = buffer.getByte();
	printType(tag_id);
	
	rx_uint32 b32  = buffer.getBigEndianU24();
	printf("Data size: %d\n", b32);
	
	b32 = buffer.getBigEndianU24();
	printf("Timestamp: %d\n", b32);
		
	rx_uint8 b8 = buffer.getByte();
	printf("Timestamp extended: %d\n", b8);

	b32 = buffer.getBigEndianU24();
	printf("Stream ID: %d\n", b32);

	switch(tag_id) {
	case FLV_TAG_SCRIPT_DATA: {
		AMFType* t = amf.parseType(buffer);
		t->print();
		delete t;
		t = NULL;

		t = amf.parseType(buffer);
		t->print();
		b32 = buffer.getBigEndianU24();
		printf("END? : %d\n", b32);
		break;
	}
	case FLV_TAG_VIDEO: {
		printf("Read video tag..\n");
		break;
	}
	default: printf("Unhandled tag type (%d)\n", tag_id); break;
	};
	printf("++++++++\n");
	return -1;
}


// W R I T I N G
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FLV::writeHeader(bool hasVideo, bool hasAudio) {
	buffer.putU8('F'); 
	buffer.putU8('L'); 
	buffer.putU8('V');
	buffer.putU8(0x01); // version

	rx_uint8 flags = 0;
	if(hasVideo) {
		flags |= 0x01; 
	}
	if(hasAudio) {
		flags |= 0x04; 
	}
	buffer.putU8(flags); // audio + video flags
	buffer.putBigEndianU32(9); // data offset
	buffer.putBigEndianU32(0); // previous tag size 0

	return buffer.size();
}

int FLV::writeMetaDataX264(x264_param_t* p) {
	int now = buffer.size();

	// tag info
	buffer.putU8(FLV_TAG_SCRIPT_DATA);
	int size_pos = buffer.size();
	buffer.putBigEndianU24(0); // data length
	buffer.putBigEndianU24(0); // timestamp
	buffer.putBigEndianU32(0); // stream id

	// meta data
	int start_data = buffer.size();
	amf.putStringAMF0(buffer, AMFString("onMetaData", true));

	buffer.putU8(AMF0_TYPE_ECMA_ARRAY);
	buffer.putBigEndianU32(7); // we have 7 elements in the buffer
	amf.putStringAMF0(buffer, AMFString("duration", false));
	amf.putNumberAMF0(buffer, AMFNumber(0));
	amf.putStringAMF0(buffer, AMFString("width", false));
	amf.putNumberAMF0(buffer, AMFNumber(1024));
	amf.putStringAMF0(buffer, AMFString("height", false));
	amf.putNumberAMF0(buffer, AMFNumber(768));
	amf.putStringAMF0(buffer, AMFString("videodatarate", false));
	amf.putNumberAMF0(buffer, AMFNumber(0));
	amf.putStringAMF0(buffer, AMFString("framerate", false));
	amf.putNumberAMF0(buffer, AMFNumber(60));
	amf.putStringAMF0(buffer, AMFString("videocodecid", false));
	amf.putNumberAMF0(buffer, AMFNumber(FLV_VIDEOCODEC_AVC));
	amf.putStringAMF0(buffer, AMFString("encoder", false));
	amf.putStringAMF0(buffer, AMFString("Lavf54.12.0", true)); 

	amf.putStringAMF0(buffer, AMFString("filesize", false));
	pos_file_size = buffer.size();
	amf.putNumberAMF0(buffer, AMFNumber(0));
	char* str = "";

	amf.putStringAMF0(buffer, AMFString("", false));
	buffer.putU8(AMF0_TYPE_OBJECT_END);

	// rewrite size
	int end_data = buffer.size();	
	rx_uint32 data_size = end_data - start_data;
	buffer.putBigEndianU24(data_size, size_pos);


	rx_uint32 tag_size = buffer.size() - now;
	buffer.putBigEndianU32(tag_size);
	return tag_size;
}

// D E B U G 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FLV::printType(rx_uint8 t) {
	switch(t) {
	case FLV_TAG_SCRIPT_DATA: printf("TagType: Script Data\n"); break;
	case FLV_TAG_AUDIO: printf("TagType: Audio\n"); break;
	case FLV_TAG_VIDEO: printf("TagType: Video\n"); break;
	default: printf("TagType: Unknown\n");
	};
}
