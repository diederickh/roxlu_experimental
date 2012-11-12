#ifndef ROXLU_GIFH
#define ROXLU_GIFH

#include <gif/GifIO.h>
#include <gif/MedianCut.h>
#include <gif/Dither.h>
#include <vector>

/*

 
  How to use:
  -----------

  roxlu::Gif* g = new roxlu::Gif(width,height, 32, 0);
  unsigned char* rgb_pixels = ... // works only with rgb
  g->addFrame(pixels, 100);
  g->save("filename.gif");

*/

using std::vector;

namespace roxlu {

  struct GifFrame {
    unsigned char* pixels;
    unsigned char* data;
    int delay;
  };

  class Gif {
  public:
    Gif(int width, int height, int numColorsInPalette = 32, int numLoop = 0);
    Gif();
    ~Gif();
	
    void setup(int width, int height);
    void addColor(unsigned char r, unsigned char g, unsigned char b);
    void addFrame(unsigned char* pixels, int delay = 0, bool useForPalette = true);
    bool save(const char* filepath);
    void createPalette(unsigned char* pixels);
	
    int width;
    int height;
    int palette_size;
    int num_loops; // 0 for infite
    const char* filepath;
	
    std::vector<MCPoint> palette;
	
  private:
    bool is_setup;
    vector<GifFrame*> frames;
    bool palette_created;
  };

} // roxlu

#endif
