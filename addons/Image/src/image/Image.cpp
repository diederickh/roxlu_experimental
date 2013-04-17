#include <roxlu/core/Utils.h>
#include <image/Image.h>

// ------------------------------------------------

Image::Image() 
  :pixels(NULL)
  ,stride(0)
  ,width(0)
  ,height(0)
  ,bits_per_pixel(0)
  ,fmt(RX_FMT_NONE)
{
}

Image::~Image() {

  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  stride = 0;
  width = 0;
  height = 0;
  bits_per_pixel = 0;
  fmt = RX_FMT_NONE;
}

Image::Image(const Image& other) 
  :pixels(NULL)
  ,stride(0)
  ,width(0)
  ,height(0)
  ,bits_per_pixel(0)
  ,fmt(RX_FMT_NONE)
{
  clone(other);
}

Image& Image::operator=(const Image& other) {

  if(&other == this) {
    return *this;
  }

  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  return clone(other);

}

Image& Image::clone(const Image& other) {

  stride = other.stride;
  width = other.width;
  height = other.height;
  bits_per_pixel = other.bits_per_pixel;
  fmt = other.fmt;

  copyPixels(other.pixels, other.width, other.height, other.fmt);

  return *this;
}

void Image::deallocate() {
  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  stride = 0;
  width = 0;
  height = 0;
  bits_per_pixel = 0;
  fmt = RX_FMT_NONE;
}

bool Image::copyPixels(unsigned char* pix, unsigned int w, unsigned int h, unsigned format) {

  if(!setPixelFormat(format)) {
    return false;
  }

  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  setWidth(w);
  setHeight(h);
  setStride(w * (bits_per_pixel / 8));
  unsigned int num_bytes =  h * stride;
  pixels = new unsigned char[num_bytes];

  if(!pixels) {
    RX_ERROR(ERR_IMG_CANNOT_ALLOCATE);
    return false;
  }
  
  memcpy(pixels, pix, num_bytes);

  

  return true;
}

bool Image::load(std::string filename, bool datapath) {
  if(pixels) {
    deallocate();
  }
  
  std::string ext = rx_get_file_ext(filename);

  if(ext == "png") {
    return rx_load_png(*this, filename, datapath);
  }
  else if(ext == "jpg") {
    return rx_load_jpg(*this, filename, datapath);
  }
  else if(ext == "tga") {
    return rx_load_tga(*this, filename, datapath);
  }
  else {
    RX_ERROR(ERR_IMG_UNSUPPORTED_EXT);
    return false;
  }

  return false;
}

bool Image::save(std::string filename, bool datapath) {
  std::string ext = rx_get_file_ext(filename);

  if(ext == "png") {
    return rx_save_png(*this, filename, datapath);
  }
  return false;
}

// ------------------------------------------------

bool rx_load_png(Image& img, std::string filename, bool datapath) {
  
  PNG png;

  if(!png.load(filename, datapath)) {
    return false;
  }
  
  switch(png.getColorType()) {
    case PNG_COLOR_TYPE_GRAY: {
      img.setPixelFormat(RX_FMT_GRAY8);
      break;
    }
    case PNG_COLOR_TYPE_RGB: {
      img.setPixelFormat(RX_FMT_RGB24);
      break;
    }
    case PNG_COLOR_TYPE_RGB_ALPHA: {
      img.setPixelFormat(RX_FMT_RGBA32);
      break;
    }
    default: {
      RX_ERROR(PNG_ERR_UNSUPPORTED_FORMAT);
      return false;
    }
  }

  img.setWidth(png.getWidth());
  img.setHeight(png.getHeight());

  return img.copyPixels(png.getPixels(), png.getWidth(), png.getHeight(), img.getPixelFormat());
}

bool rx_load_jpg(Image& img, std::string filename, bool datapath) {

  JPG jpg;

  if(!jpg.load(filename, datapath)) {
    return false;
  }

  if(jpg.getNumChannels() == 3) {
    img.setPixelFormat(RX_FMT_RGB24);
  }
  else {
    RX_ERROR(ERR_IMG_UNSUPPORTED_FORMAT);
    return false;
  }

  img.setWidth(jpg.getWidth());
  img.setHeight(jpg.getHeight());

  return img.copyPixels(jpg.getPixels(), jpg.getWidth(), jpg.getHeight(), img.getPixelFormat());

}

bool rx_load_tga(Image& img, std::string filename, bool datapath) {

  TGA tga;
  
  if(!tga.load(filename, datapath)) {
    return false;
  }

  if(tga.getNumChannels() == 4) {
    img.setPixelFormat(RX_FMT_RGBA32);
  }
  else if(tga.getNumChannels() == 3) {
    img.setPixelFormat(RX_FMT_RGB24);
  }
  else {
    RX_ERROR(ERR_IMG_UNSUPPORTED_FORMAT);
    return false;
  }

  img.setWidth(tga.getWidth());
  img.setHeight(tga.getHeight());
  
  return img.copyPixels(tga.getPixels(), tga.getWidth(), tga.getHeight(), img.getPixelFormat());
}

// ------------------------------------------------

bool rx_save_png(Image& img, std::string filename, bool datapath) {
  
  PNG png;

  png_uint_32 type;

  if(img.getPixelFormat() == RX_FMT_GRAY8) {
    type = PNG_COLOR_TYPE_GRAY;
  }
  else if(img.getPixelFormat() == RX_FMT_RGB24) {
    type = PNG_COLOR_TYPE_RGB;
  }
  else if(img.getPixelFormat() == RX_FMT_RGBA32) {
    type = PNG_COLOR_TYPE_RGB_ALPHA;
  }
  else {
    RX_ERROR(ERR_IMG_UNSUPPORTED_FORMAT);
    return false;
  }

  png.setPixels(img.getPixels(), img.getWidth(), img.getHeight(), type);
  png.save(filename, datapath);

  return true;
}
