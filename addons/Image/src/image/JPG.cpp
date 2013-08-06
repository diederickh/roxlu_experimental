#include <stdlib.h>
#include <stdio.h>
 // #if defined(_WIN32)
 // #include <windows.h>
 // #endif

#include <image/JPG.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>        

JPG::JPG() 
  :pixels(NULL)
  ,width(0)
  ,height(0)
  ,stride(0)
  ,bit_depth(0)
  ,num_channels(0)
  ,color_space(JCS_UNKNOWN)
  ,dct_method(JDCT_DEFAULT) 
  ,dither_mode(JDITHER_NONE)
  ,quality(80)
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

  color_space = other.color_space;
  dct_method = other.dct_method;
  dither_mode = other.dither_mode;
  
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

bool JPG::setPixels(unsigned char* pix, int w, int h, J_COLOR_SPACE type) {

  if(!pix) {
    RX_ERROR("Invalid pixels given");
    return false;
  }

  if(!w) {
    RX_ERROR("Invalid width given");
    return false;
  }

  if(!h) {
    RX_ERROR("invalid height given");
    return false;
  }

  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  color_space = type;
  width = w;
  height = h;
  
  switch(color_space) {
    case JCS_RGB: {
      num_bytes = width * height * 3;
      num_channels = 3;
      stride = width * 3;
      bit_depth = 8; 
      break;
    }
    default: {
      RX_ERROR("Unhandled color space: %d", type);
      return false;
    }
  }

  pixels = new unsigned char[num_bytes];
  if(!pixels) {
    RX_ERROR("Cannot allocate %ld bytes for the jpg", num_bytes);
    return false;
  }

  memcpy((char*)pixels, (char*)pix, num_bytes);

  return true;
}

bool JPG::save(std::string filename, bool datapath) {

  if(!width || !height) {
    RX_ERROR("Invalid width or height: %d x %d", width, height);
    return false;
  }
  
  if(!stride) {
    RX_ERROR("Invalid stride: %d", stride);
    return false;
  }
  
  if(!pixels) {
    RX_ERROR("No pixels set to save");
    return false;
  }

  if(!bit_depth) {
    RX_ERROR("Invalid bit_depth: %d", bit_depth);
    return false;
  }

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  FILE* fp = fopen(filename.c_str(), "wb");
  if(!fp) {
    RX_ERROR("Cannot open the file: `%s`", filename.c_str());
    fp = NULL;
    return false;
  }

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  
  jpeg_stdio_dest(&cinfo, fp);

  // compression parameters
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = num_channels;
  cinfo.in_color_space = color_space;
  
  jpeg_set_defaults(&cinfo); // after setting the default we can set our custom settings
  
  // @todo > set compression level, dither, dct

  jpeg_set_quality(&cinfo, quality, TRUE /* limit to jpeg baseline values */);

  jpeg_start_compress(&cinfo, TRUE /* write complete data stream */); 

  while(cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &pixels[cinfo.next_scanline * stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);

  fclose(fp);
  fp = NULL;

  jpeg_destroy_compress(&cinfo);
    
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

  color_space = JCS_UNKNOWN;
  dct_method = JDCT_DEFAULT;
  dither_mode = JDITHER_NONE;
    
}
