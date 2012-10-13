
#include "PCMWriter.h"

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
	printf("Buffer size: %zu\n", ring_buffer.size());
	size_t written = fwrite(ring_buffer.getReadPtr(), sizeof(char), ring_buffer.size(), fp);
	printf("Written to file: %zu\n", written);
	fclose(fp);
}

void PCMWriter::onAudioIn(const void* input, unsigned long numFrames) {
	printf(">  %lu\n", numFrames);
	ring_buffer.write((char*)input, numFrames * sizeof(short int));
	/*
	const short int* data = (const short int*)(input);
	const float* fdata = (const float*)(input);
	for(int i = 0; i < numFrames; ++i) {
		short int d = data[i];
		float f = fdata[i];
		printf("%d - %f\n", d, f);
	}
	printf("\n");
	*/
}
