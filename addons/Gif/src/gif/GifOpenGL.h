#ifndef ROXLU_GIF_OPENGLH
#define ROXLU_GIF_OPENGLH

#include <roxlu/Roxlu.h>
#include <gif/Gif.h>

namespace roxlu {
  class GifOpenGL {
  public:
    GifOpenGL();
    ~GifOpenGL();
    void grabFrame();
    bool save(const char* filepath);
  private:
    Gif gif;
    bool is_setup;
    unsigned char* pixels;
    unsigned char* dest_pixels;
  };

}
#endif
