#include <audio/PCMWriter.h>

// Just added this as an test.... might be removed

PCMWriter::PCMWriter() 
	:ring_buffer(1024 * 100)
{
	fp = fopen("raw_16bit_16hz.pcm", "w");
	if(!fp) {
		printf("ERROR: cannot open file..\n");
		::exit(1);
	}
}

PCMWriter::~PCMWriter() {
	size_t written = fwrite(ring_buffer.getReadPtr(), sizeof(char), ring_buffer.size(), fp);
	printf("Written to file: %zu\n", written);
	fclose(fp);
}

void PCMWriter::onAudioIn(const void* input, unsigned long numFrames) {
	ring_buffer.write((char*)input, numFrames * sizeof(short int));
}
