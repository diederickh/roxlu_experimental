#include <image/PNG.h>
#include <roxlu/core/Utils.h>
#include <png.h>

// ----------------------------------------------------------------

PNG::PNG() 
  :width(0)
  ,height(0)
  ,bit_depth(0)
  ,color_type(999999)
  ,pixels(NULL)
  ,num_bytes(0)
  ,stride(0)
  ,num_channels(0)
{
}

PNG::PNG(const PNG& other)
  :pixels(NULL)
{
  if(other.num_bytes > 0 && other.pixels) {
    pixels = new unsigned char[other.num_bytes];
    memcpy((char*)pixels, (char*)other.pixels, other.num_bytes);
  }
  width = other.width;
  height = other.height;
  bit_depth = other.bit_depth;
  color_type = other.color_type;
  num_bytes = other.num_bytes;
  stride = other.stride;
  num_channels = other.num_channels;
}

PNG& PNG::operator=(const PNG& other) {
  if(pixels) {
    delete[] pixels;
  }

  if(other.num_bytes > 0) {
    pixels = new unsigned char[other.num_bytes];
    memcpy((char*)pixels, (char*)other.pixels, other.num_bytes);
  }

  width = other.width;
  height = other.height;
  bit_depth = other.bit_depth;
  color_type = other.color_type;
  num_bytes = other.num_bytes;
  stride = other.stride;
  num_channels = other.num_channels;
  return *this;
}

PNG::~PNG() {
  width = 0;
  height = 0;
  num_bytes = 0;
  color_type = 9999;
  stride = 0;
  bit_depth = 0;
  num_channels = 0;

  if(pixels) {
    delete[] pixels;
  }

  pixels = NULL;
}

bool PNG::load(std::string filename, bool datapath) {

#if defined(_WIN32) && defined(ROXLU_DEBUG)
  RX_VERBOSE("The png library results in wierd behaviour when compiling on windows in debug mode");
#endif  

  png_structp png_ptr;
  png_infop info_ptr; 

  if(pixels) {
    RX_ERROR(PNG_ERR_ALREADY_LOADED);
    return false;
  }
  
  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  FILE* fp = fopen(filename.c_str(), "rb");
  if(!fp) {
    RX_ERROR(PNG_ERR_LOAD_NO_FILE, filename.c_str());
    fp = NULL;
    return false;
  }

  unsigned char sig[8];
  size_t r = 0;
  r = fread(sig, 1, 8, fp);
  if(r != 8) {
    RX_ERROR(PNG_ERR_CANNOT_READ);
    fclose(fp);
    fp = NULL;
    return  false;
  }

  if(!png_check_sig(sig, 8)) {
    RX_ERROR(PNG_ERR_SIG);
    fclose(fp);
    fp = NULL;
    return false;
  }
  
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png_ptr) {
    RX_ERROR(PNG_ERR_CREATE_READ_ST);
    fclose(fp);
    fp = NULL;
    return false;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    RX_ERROR(PNG_ERR_CREATE_INFO_PTR);
    fclose(fp);
    fp = NULL;
    return false;
  }

  #if !defined(_WIN32)
  if(setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;
    return false;
  }
  #endif

  // @TODO - add option to rescale to 8bit color info or 16bit
  // @TODO - add option to strip the alpha (not recommended in the example)
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  num_channels = png_get_channels(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);

  if(width == 0 || height == 0) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;
    return false;
  }

  // @TODO - add option to allow input colors/gray values to be not converted
  switch(color_type) {
    case PNG_COLOR_TYPE_PALETTE: {
      png_set_palette_to_rgb(png_ptr);
      num_channels = 3;
      break;
    }
    case PNG_COLOR_TYPE_GRAY: {
      if(bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
        bit_depth = 8;
      }
      break;
    }
    default:break;
  };
  
  // When transparency is set convert it to a full alpha channel
  if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
    num_channels += 1;
  }

  stride = width * bit_depth * num_channels / 8;  
  num_bytes = width * height * bit_depth * num_channels / 8;

  pixels = new unsigned char[num_bytes];
  if(!pixels) {
    RX_ERROR(PNG_ERR_IMG_TOO_BIG);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;
    pixels = NULL;
    return false;
  }

  png_bytep* row_ptrs = new png_bytep[height];
  if(!row_ptrs) {
    RX_ERROR(PNG_ERR_IMG_TOO_BIG);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;
    delete[] pixels;
    pixels = 0;
    return false;
  }

  for(size_t i = 0; i < height; ++i) {
     row_ptrs[i] = (png_bytep) pixels +(i * stride);
  }

  png_read_image(png_ptr, row_ptrs);

  delete[] row_ptrs;
  row_ptrs = NULL;
  png_destroy_read_struct(&png_ptr, &info_ptr, 0);
  fclose(fp);
  return true;
}

bool PNG::save(std::string filename, bool datapath) {

  if(!width || !height) {
    RX_ERROR(PNG_ERR_SIZE, width, height);
    return false;
  }

  if(!stride) {
    RX_ERROR(PNG_ERR_STRIDE, stride);
    return false;
  }

  if(!pixels) {
    RX_ERROR(PNG_ERR_PIXELS);
    return false;
  }

  if(!bit_depth) {
    RX_ERROR(PNG_ERR_BITDEPTH, bit_depth);
    return false;
  }

  png_structp png_ptr; 
  png_infop info_ptr; 

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  FILE* fp = fopen(filename.c_str(), "wb");
  if(!fp) {
    RX_ERROR(PNG_ERR_SAVE_NO_FILE, filename.c_str());
    fp = NULL;
    return false;
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(png_ptr == NULL) {
    RX_ERROR(PNG_ERR_CREATE_WRITE_ST);
    fclose(fp);
    fp = NULL;
    return false;
  }
  

  info_ptr = png_create_info_struct(png_ptr);
  if(info_ptr == NULL) {
    RX_ERROR(PNG_ERR_CREATE_INFO_PTR);
    fclose(fp);
    fp = NULL;
    return false;
  }

  if(setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    fp = NULL;
    return false;
  }

  png_set_IHDR(png_ptr, 
               info_ptr, 
               width, 
               height, 
               bit_depth, 
               color_type, 
               PNG_INTERLACE_NONE, 
               PNG_COMPRESSION_TYPE_DEFAULT, 
               PNG_FILTER_TYPE_DEFAULT);

  png_bytep* row_ptrs = new png_bytep[height];
  
 
  for(size_t j = 0; j < height; ++j) {
    row_ptrs[j] = pixels + (j * stride);
  }

  png_init_io(png_ptr, fp);
  png_set_rows(png_ptr, info_ptr, row_ptrs);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  delete[] row_ptrs;

  fclose(fp);

  return true;
}

// for now we assume 8 bits per channel
bool PNG::setPixels(unsigned char* pix, png_uint_32 w, png_uint_32 h, png_uint_32 type) {
  if(pixels) {
    delete[] pixels;
  }

  width = w;
  height = h;
  color_type = type;
  bit_depth = 8; 
  
  switch(color_type) {
    case PNG_COLOR_TYPE_GRAY: {
      num_bytes = width * height;
      num_channels = 1;
      stride = width;
      break;
    }
    case PNG_COLOR_TYPE_GRAY_ALPHA: {
      num_bytes = width * height * 2;
      num_channels = 2;
      stride = width * 2;
      break;
    }
    case PNG_COLOR_TYPE_RGB: {
      num_bytes = width * height * 3;
      num_channels = 3;
      stride = width * 3;
      break;
    }
    case PNG_COLOR_TYPE_RGB_ALPHA: {
      num_bytes = width * height * 4;
      num_channels = 4;
      stride = width * 4;
      break;
    }
    default: return false;
  };

  pixels = new unsigned char[num_bytes];
  memcpy((char*)pixels, (char*)pix, num_bytes);

  return true;
}


void PNG::print() {
  RX_VERBOSE(PNG_V_WIDTH, width);
  RX_VERBOSE(PNG_V_HEIGHT, height);
  RX_VERBOSE(PNG_V_BITDEPTH, bit_depth);
  RX_VERBOSE(PNG_V_NCHANNELS, num_channels);
  RX_VERBOSE(PNG_V_COLORTYPE, colorTypeToString(color_type).c_str());
  RX_VERBOSE(PNG_V_NUMBYTES, num_bytes);
}

