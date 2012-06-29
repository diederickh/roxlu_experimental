#include "Webp.h"
WebP::WebP() 
	:loaded(false)
{
	if(!WebPInitDecoderConfig(&config)) {
		printf("Wrong webp library.\n");
	}
}

WebP::WebP(const string& filename) {
	WebP();
	load(filename);
}

WebP::~WebP() {
	if(loaded) {
		WebPFreeDecBuffer(&config.output);
		loaded = false;
	}
}

// we assume files to be in data path.
bool WebP::load(const string& filename) {
	if(loaded) {
		WebPFreeDecBuffer(&config.output);
		loaded = false;
	}
	
	string filepath = File::toDataPath(filename);
	printf("Loading: %s\n", filepath.c_str());
	WebPDecBuffer* const output_buffer = &config.output;
	WebPBitstreamFeatures* const bitstream = &config.input;
	VP8StatusCode status = VP8_STATUS_OK;
	int ok;
	size_t data_size;
	FILE* const in = fopen(filepath.c_str(), "rb");
	
	if(!in) {
		printf("Cannot open file: %s\n", filepath.c_str());
		return false;
	}
	
	fseek(in, 0, SEEK_END);
	data_size = ftell(in);
	fseek(in, 0, SEEK_SET);
	printf("Data size: %zu\n", data_size);
	unsigned char* data = new unsigned char[data_size];	
	ok = (fread(data, data_size, 1, in) == 1);
	if(!ok) {
		printf("Cannot read from webp image.\n");
		return false;
	}
	
	status = WebPGetFeatures((const uint8_t*)data, data_size, bitstream);
	if(status != VP8_STATUS_OK) {
		delete[] data;
		data = NULL;
		return false;
	}
	
	output_buffer->colorspace = MODE_RGBA;
	status = WebPDecode((const uint8_t*)data, data_size, &config);
	if(status != VP8_STATUS_OK) {
		return false;
	}
	loaded = true;
	delete[] data;
	data = NULL;
	return true;
}

