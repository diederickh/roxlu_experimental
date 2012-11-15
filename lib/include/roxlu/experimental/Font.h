#ifndef ROXLU_STB_FONTH
#define ROXLU_STB_FONTH

#include <roxlu/Roxlu.h>
#include <stdio.h>

#undef STB_TRUETYPE_IMPLEMENTATION
#include <roxlu/external/stb_truetype.h>

namespace roxlu {
class Font {
 public:
  Font();
  ~Font();
  bool open(const std::string& filepath);
 private:
  int w;
  int h;
  stbtt_bakedchar* cdata;
};

} // roxlu
#endif
