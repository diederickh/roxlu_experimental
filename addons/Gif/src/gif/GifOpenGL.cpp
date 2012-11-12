#include <gif/GifOpenGL.h>

GifOpenGL::GifOpenGL() 
  :is_setup(false)
  ,pixels(NULL)
  ,dest_pixels(NULL)
{
}

GifOpenGL::~GifOpenGL() {
  if(pixels) {
    delete[] pixels;
    delete[] dest_pixels;
  }
}

bool GifOpenGL::save(const char* filepath) {
  return gif.save(filepath);
}

void GifOpenGL::grabFrame() {
  if(!is_setup) {
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport); eglGetError();
    gif.setup(viewport[2], viewport[3]); eglGetError();
    is_setup = true;
    pixels = new unsigned char[gif.width * gif.height * 3];
    dest_pixels = new unsigned char[gif.width * gif.height * 3];
  }

  glReadBuffer(GL_BACK); eglGetError();
  glReadPixels(0, 0, gif.width, gif.height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);  eglGetError();
  int stride = gif.width * 3;
  for(int j = 0; j < gif.height; ++j) {
    int src_dx = ((gif.height - j) * gif.width * 3);
    int dest_dx =  j * gif.width * 3;
    memcpy(dest_pixels + dest_dx, pixels + src_dx, stride);
  }
  gif.addFrame(dest_pixels);
}
