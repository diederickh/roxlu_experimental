#include <roxlu/experimental/Image.h>

#undef STBI_HEADER_FILE_ONLY
#include <roxlu/external/stb_image.c>

#undef STB_IMAGE_WRITE_IMPLEMENTATION
#include <roxlu/external/stb_image_write.h>

namespace roxlu {

  Image::Image() 
    :width(0)
    ,height(0)
    ,pixels(NULL)
    ,bpp(0)
  {

  }

  Image::~Image() {
    if(pixels != NULL) {
      delete[] pixels;
      pixels = NULL;
    }
  }

  bool Image::load(const std::string& filepath) {
    pixels = stbi_load(filepath.c_str(), &width, &height, &bpp, 0);
    if(!pixels) {
      return false;
    }
    return true;
  }

  bool Image::save(const std::string& filepath) {
    if(pixels != NULL) {
      return stbi_write_png(filepath.c_str(), width, height, bpp, pixels, bpp * width) > 0;
    }
    return true;
  }


} // roxlu
