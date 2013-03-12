#ifndef ROXLU_BMFONT_TYPES_H
#define ROXLU_BMFONT_TYPES_H

#include <string>

struct BMFVertex {
  BMFVertex();
  BMFVertex(float x, float y, float s, float t);
  ~BMFVertex();

  void set(float x, float y, float s, float t);
  void setColor(float r, float g, float b, float a = 1.0);
  void setColor4fv(float* c);
  
  void clear();
  char* getPtr();
 
  float position[2];
  float texcoord[2];
  float color[4];
};

struct BMFChar {
  BMFChar();
  ~BMFChar();
  void clear();

  size_t id;
  int x;
  int y;
  int width;
  int height;
  int xoffset;
  int yoffset; 
  int xadvance;
};


#endif
