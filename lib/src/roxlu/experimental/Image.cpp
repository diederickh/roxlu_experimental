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
      printf("REMOVE!\n");
      delete[] pixels;
      pixels = NULL;
      printf("D\n");
    }
  }

  bool Image::load(const char* filepath) {
    pixels = stbi_load(filepath, &width, &height, &bpp, 0);
    if(!pixels) {
      return false;
    }
    return true;
  }

  bool Image::save(const char* filepath) {
    if(pixels != NULL) {
      return stbi_write_png(filepath, width, height, bpp, pixels, bpp * width) > 0;
    }
    return true;
  }


} // roxlu
