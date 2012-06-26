#include <fstream>
#include "TGA.h"

TGA::TGA()
	:pixels(NULL)
	,width(0)
	,height(0)
	,bpp(0)
	,id(0)
{
}

TGA::~TGA() {
	if(pixels != NULL) {
		delete[] pixels;
	}
}


// super naive loading of uncompressed targa images
bool TGA::load(const string& filepath) {
	std::ifstream ifs;
	ifs.open(filepath.c_str(), std::ios::binary | std::ios::ate);
	if(!ifs.is_open()) {
		printf("Cannot open: '%s'\n", filepath.c_str());
		return false;
	}
	
	ifs.seekg(0, std::ios::beg);
	ifs.read((char*)header, sizeof(unsigned char)*12);
	ifs.read((char*)&width, sizeof(unsigned short));
	ifs.read((char*)&height, sizeof(unsigned short));
	ifs.read((char*)&bpp, sizeof(unsigned char));
	ifs.read((char*)&id, sizeof(unsigned char));
	
	num_bytes = (uint32_t)width * height * bpp;	
	if(pixels != NULL) {
		delete[] pixels;
	}
	pixels = new unsigned char[num_bytes];
	ifs.read((char*)pixels, num_bytes);
	
	ifs.close();
	return true;
}

void TGA::print() {
	printf("header: %s\n", header);
	printf("width: %d\n", width);
	printf("height: %d\n", height);
	printf("bpp: %d\n", bpp);
	printf("id: %d\n", id);
	printf("num bytes: %zu\n", num_bytes);	
	
	return;
	
	for(int j = 0; j < height; ++j) {
		for(int i = 0; i < width; ++i) {
			int dx = j * width * bpp + i * bpp;
			printf("(%d, %d, %d, %d), ", pixels[dx], pixels[dx+1], pixels[dx+2], pixels[dx+3]);
		}
		if(j > 2) {
			break;
		}
	}
}