#include <bmfont/BMTypes.h>

// -------------------------------------------
BChar::BChar() {
  clear();
}

BChar::~BChar() {
  clear();
}

void BChar::clear() {
  id = 0;
  x = 0;
  y = 0;
  width = 0;
  height = 0;
  xoffset = 0;
  yoffset = 0;
  xadvance = 0;
}

// -------------------------------------------

BVertex::BVertex() {
  clear();
}

BVertex::BVertex(float x, float y, float s, float t) {
  clear();
  set(x, y, s, t);
}

BVertex::~BVertex() {
  clear();
}

void BVertex::clear() {
  position[0] = position[1] = 0.0f;
  texcoord[0] = texcoord[1] = 0;
  color[0] = color[1] = color[2] = color[3] = 1.0f;
}

void BVertex::set(float x, float y, float s, float t) {
  position[0] = x;
  position[1] = y;
  texcoord[0] = s;
  texcoord[1] = t;
}

void BVertex::setColor(float r, float g, float b, float a) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}

void BVertex::setColor4fv(float* v) {
  setColor(v[0], v[1], v[2], v[3]);
}

char* BVertex::getPtr() {
  return (char*)&position[0];
}


