#include <stdlib.h>
#include <stdio.h>
 // #if defined(_WIN32)
 // #include <windows.h>
 // #endif
#if defined(_WIN32)
#  define XMD_H
#  include <jpeglib.h>
#  undef XMD_H
#else 
#  include <jpeglib.h>
#endif
#include <image/JPG.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>        

JPG::JPG() 
  :pixels(NULL)
{
  clear();
}

JPG::JPG(const JPG& other) 
  :pixels(NULL)
{
  clone(other);
}

JPG& JPG::operator=(const JPG& other) {
  return clone(other);
}

JPG& JPG::clone(const JPG& other) {
  clear();

  if(other.pixels && other.num_bytes > 0) {
    pixels = new unsigned char[other.num_bytes];
    memcpy(pixels, other.pixels, other.num_bytes);
  }

  num_bytes = other.num_bytes;
  stride = other.stride;
  width = other.width;
  height = other.height;
  bit_depth = other.bit_depth;
  num_channels = other.num_channels;
  
  return *this;
}

JPG::~JPG() {
  clear();
}

bool JPG::load(std::string filename, bool datapath) {
  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;
  FILE* fp;
  JSAMPARRAY buffer;
  
  if( (fp = fopen(filename.c_str(), "rb")) == NULL ) {
    RX_ERROR(ERR_JPG_FILE_NOT_OPENED, filename.c_str());
    return false;
  }

  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  stride = cinfo.output_width * cinfo.output_components;
  num_channels = cinfo.output_components;
  width = cinfo.output_width;
  height = cinfo.output_height;
  bit_depth = 8;
  num_bytes = width * height * num_channels;

  pixels = new unsigned char[num_bytes];
  if(!pixels) {
    RX_ERROR(ERR_JPG_CANNOT_ALLOC);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return false;
  }

  size_t dest_row = 0;
  buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, stride, 1);
  while(cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(pixels + (dest_row * stride), buffer[0], stride);
    dest_row++;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);
  
  return true;
}

// decompress from memory stream
bool JPG::load(unsigned char* compressed, size_t nbytes) {
  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;
  
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, compressed, nbytes);

  int rc = jpeg_read_header(&cinfo, TRUE);
  if(rc != 1) {
    RX_ERROR("Error while reading the jpeg header");
    return false;
  }

  bool need_alloc = false;
  jpeg_start_decompress(&cinfo);

  if(cinfo.output_width != width) {
    width = cinfo.output_width;
    need_alloc = true;
  }
  if(cinfo.output_height != height) {
    height = cinfo.output_height;
    need_alloc = true;
  }
  if(cinfo.output_components != num_channels) {
    num_channels = cinfo.output_components;
    need_alloc = true;
  }

  if(!width || !height) {
    RX_ERROR("Read incorrect jpg size: %d x %d", width, height);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return false;
  }

  // only allocate when the sizes or num channels change.
  if(need_alloc) {
    bit_depth = 8;
    num_bytes = width * height * num_channels;
    stride = width * num_channels;
    pixels = new unsigned char[num_bytes];
  }

  size_t dest_row = 0;
  while(cinfo.output_scanline < cinfo.output_height) {
    unsigned char* buffer_array[1];
    buffer_array[0] = pixels + cinfo.output_scanline * stride;
    jpeg_read_scanlines(&cinfo, buffer_array, 1);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
 
  return true;
}

void JPG::print() {
  RX_VERBOSE("JPG.width: %d", width);
  RX_VERBOSE("JPG.height: %d", height);
  RX_VERBOSE("JPG.bit_depth: %d", bit_depth);
  RX_VERBOSE("JPG.num_channels: %d", num_channels);
}

void JPG::clear() {
  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  num_bytes = 0;
  stride = 0;
  width = 0;
  height = 0;
  bit_depth = 0;
  num_channels = 0;
}
