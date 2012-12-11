#include <gif/Gif.h>

namespace roxlu {

  Gif::Gif(int width, int height, int numColorsInPalette, int loop)
    :width(width)
    ,height(height)
    ,palette_size(numColorsInPalette)
    ,palette_created(false)
    ,num_loops(loop)
    ,is_setup(true)
  {

  }

  Gif::Gif() 
    :width(0)
    ,height(0)
    ,palette_size(32)
    ,palette_created(false)
    ,num_loops(0)
    ,is_setup(false)
  {
  }

  Gif::~Gif() {
  }

  void Gif::addColor(unsigned char r, unsigned char g, unsigned char b) {
    MCPoint p;
    p.x[0] = r;
    p.x[1] = g;
    p.x[2] = b;
    palette.push_back(p);
  }

  void Gif::setup(int w, int h) {
    width = w;
    height = h;
    is_setup = true;
  }

  void Gif::addFrame(unsigned char* pixels, int delay, bool useForPalette) {
    if(!is_setup) {
      return;
    }
	
    // add a new frame to the queue.
    GifFrame* frame = new GifFrame();
    frame->pixels = new unsigned char[width*height*3];
    frame->data = new unsigned char[width*height]; 
    memcpy(frame->pixels, pixels, width*height*3 * sizeof(unsigned char));	
    frame->delay = delay;
	
    // create palette when not yet created and when asked to do.
    if(useForPalette) {
      createPalette(pixels);
    }
    printf("Frames: %zu\n", frames.size());
    frames.push_back(frame);
  }

  void Gif::createPalette(unsigned char* pixels) {
    unsigned char* copy = new unsigned char[width*height*3];
    memcpy(copy, pixels, width*height*3 * sizeof(unsigned char));	
    palette = medianCut((MCPoint*)copy, width*height, palette_size);
    delete[] copy;
    palette_created = true;
  }

  bool Gif::save(const char* filepath) {
    if(!frames.size()) {
      return false;
    }
	
    if(!palette_created) {
      createPalette(frames[0]->pixels);
    }
	
	
    FILE* fp;
    fp = fopen(filepath,"wb");
    GIFEncodeHeader(fp, 1, width, height, 0, 8,(unsigned char*)palette[0].x);
    GIFEncodeCommentExt(fp,(char*) "Roxlu lib");
	
    // write frames.
    Dither dither;
    int disposal = DISPOSE_COMBINE;
    //typedef enum GIFDisposeType {DISPOSE_UNSPECIFIED, DISPOSE_COMBINE, DISPOSE_REPLACE } GIFDisposeType;
    vector<GifFrame*>::iterator it = frames.begin();
    while(it != frames.end()) {
      GifFrame& f = *(*it);
      dither.dither(width, height, f.pixels, f.data, palette);
      GIFEncodeGraphicControlExt(fp, (GIFDisposeType)disposal,f.delay, (f.delay) > 0 ? 1 : 0,-1);
      GIFEncodeImageData (fp, width, height, 8, 0, 0, f.data);
      ++it;
    }
	
    GIFEncodeLoopExt(fp, num_loops);
    GIFEncodeClose(fp);

    fclose(fp);
    return true;
  }
 

}
