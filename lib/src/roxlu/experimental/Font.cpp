#include <roxlu/experimental/Font.h>

Font::Font() 
  :w(512)
  ,h(512)
  ,cdata(NULL)
{
}

Font::~Font() {
  if(cdata != NULL) {
    delete[] cdata;
    cdata = NULL;
  }
}

bool Font::open(const std::string& filepath) {
  // READ TTF DATA
  // -------------
  FILE* fp = fopen(filepath.c_str(), "rb");
  if(!fp) {
    printf("ERROR: cannot open: %s\n", filepath.c_str());
    return false;
  }
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* font_buffer = new unsigned char[size];
  if(!font_buffer) {
    printf("ERROR: cannot allocate font buffer.\n");
    return false;
  }

  fread(font_buffer, 1, size, fp);
  fclose(fp);
  fp = 0;

  // CREATE FONT BITMAP
  // -------------------
  unsigned char* pixels = new unsigned char[int(w * h)];
  if(!pixels) {
    printf("ERROR: cannot allocate font pixel buffer.\n");
    return false;
  }

  int char_start = 32;
  int char_end = 96;
  int num_chars = char_end - char_start;
  cdata = new stbtt_bakedchar[num_chars * 2]; // when using num_chars I get a  malloc: *** error for object 0x1008e2808: incorrect checksum for freed object - object was probably modified after being freed.

  stbtt_BakeFontBitmap(font_buffer, 0, 12.0f, pixels, w, h, char_start, char_end, cdata);

  /*
  roxlu::Image img;
  img.copyPixels(pixels, w, h, 1);
  img.save(File::toDataPath("font.png").c_str());
  */

  delete[] pixels;
  delete[] font_buffer;
  pixels = NULL;
  font_buffer = NULL;
  return true;
}
