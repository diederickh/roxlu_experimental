/**
 
 Basic BMFont parser.
 ---------------------

 - use the BMFont application to generate a font map + XML output. 
   http://www.angelcode.com/products/bmfont/

 - supports only one page font textures (make the size of the texture big enough)

 - supports a custom callback which is called when we generate triangles. You can use
   this callback to change the color of e.g. character

 - The `BMFont` class is a simple wrapper which ties the `BMFontLoader` and `BMFontRenderer` 
   together. You can use the `BMFontLoader` separately if you want ot handly the vertices 
   in a different way. The `BMFontRenderer` uses an optimized openGL render path using 
   multidraw arrays and a stream draw VBO (could be more optimized by mapping the VBO 
   memory to pinned space).


 - This class is to be used in a way where you "add" texts vertices to a buffer. You can 
   either add text once (when it's static), or update it for every frame. If you text changes
   you need to add it again and make sure to call `BMFont::reset()`.


**/


#ifndef ROXLU_BMFONT_H
#define ROXLU_BMFONT_H

#include <vector>
#include <string>
#include <bmfont/BMFontLoader.h>
#include <bmfont/BMFontRenderer.h>

class BMFont {
 public:
  BMFont();
  ~BMFont();
  bool setup(std::string filename, int windowW, int windowH, bool datapath = false);     
  void setColor(float r, float g, float b, float a = 1.0);
  void addText(std::string str, float x, float y);
  void draw();
  void reset();
  void print();
 private:
  BMFontLoader loader;
  BMFontRenderer renderer;
};

inline bool BMFont::setup(std::string filename, int windowW, int windowH, bool datapath) {
  bool r = true;
  r = loader.load(filename, datapath);
  renderer.setup(windowW, windowH);
  return r;
}

inline void BMFont::setColor(float r, float g, float b, float a) {
  loader.setColor(r, g, b, a);
}

inline void BMFont::addText(std::string str, float x, float y) {
  std::vector<BVertex> vertices = loader.generateVertices(str, x, y);
  renderer.addVertices(vertices);
}

inline void BMFont::draw() {
  renderer.update();
  renderer.draw();
}

inline void BMFont::print() {
  loader.print();
}

inline void BMFont::reset() {
  renderer.reset();
}

#endif
