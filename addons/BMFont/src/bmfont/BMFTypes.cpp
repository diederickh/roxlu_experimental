#include <bmfont/BMFTypes.h>

// -------------------------------------------
BMFChar::BMFChar() {
  clear();
}

BMFChar::~BMFChar() {
  clear();
}

void BMFChar::clear() {
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

BMFVertex::BMFVertex() {
  clear();
}

BMFVertex::BMFVertex(float x, float y, float s, float t) {
  clear();
  set(x, y, s, t);
}

BMFVertex::~BMFVertex() {
  clear();
}

void BMFVertex::clear() {
  position[0] = position[1] = 0.0f;
  texcoord[0] = texcoord[1] = 0;
  color[0] = color[1] = color[2] = color[3] = 1.0f;
}

void BMFVertex::set(float x, float y, float s, float t) {
  position[0] = x;
  position[1] = y;
  texcoord[0] = s;
  texcoord[1] = t;
}

void BMFVertex::setColor(float r, float g, float b, float a) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}

void BMFVertex::setColor4fv(float* v) {
  setColor(v[0], v[1], v[2], v[3]);
}

char* BMFVertex::getPtr() {
  return (char*)&position[0];
}


