#include "JPEGImage.h"
#include <stdlib.h>
#include <iostream>


using std::cout;
using std::endl;

namespace roxlu {
JPEGImage::JPEGImage():Image()
{
}

JPEGImage::JPEGImage(string filePath):Image() {
	loadImage(filePath);
}

bool JPEGImage::loadImage(string filePath) {
	struct jpeg_decompress_struct decomp_info;
	struct jpeg_error_mgr error_mgr;
	JSAMPROW row_pointer[1]; 
	
	// open the file.
	FILE* in_file = fopen(filePath.c_str(), "rb");

	if(!in_file) {
		printf("Error loading jpg file: %s\n", filePath.c_str());
		return false;
	}
	
	decomp_info.err = jpeg_std_error(&error_mgr);
	jpeg_create_decompress(&decomp_info);
	jpeg_stdio_src(&decomp_info, in_file);
	jpeg_read_header(&decomp_info, TRUE);
	
	// set some specs.
	width = decomp_info.image_width;
	height = decomp_info.image_height;
	bytes_per_pixel = decomp_info.num_components;	

	// decompress.	
	jpeg_start_decompress(&decomp_info);
	int scan_width = width * bytes_per_pixel;
	int i = 0;
	int dx = 0;

	unsigned char* raw_image = new unsigned char[width*height*bytes_per_pixel];
	row_pointer[0] = new unsigned char[width * bytes_per_pixel];
	while(decomp_info.output_scanline < height) {
		jpeg_read_scanlines(&decomp_info, row_pointer,1);
		for(i = 0; i < scan_width; ++i) { 
			raw_image[dx++] = row_pointer[0][i];
		}
	}
	
	// cleanup
	jpeg_finish_decompress(&decomp_info);
	jpeg_destroy_decompress(&decomp_info);
	
	fclose(in_file);
	raw_pixels = raw_image;
	return true;
}

} // roxlu