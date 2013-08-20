/*
 
  Basic BMFFont parser.
  ---------------------

  - use the BMFFont application to generate a font map + XML output. 
  http://www.angelcode.com/products/bmfont/

  - supports only one page font textures (make the size of the texture big enough)

  - supports a custom callback which is called when we generate triangles. You can use
  this callback to change the color of e.g. character

  - The `BMFFont` class is a simple wrapper which ties the `BMFFontLoader` and `BMFFontRenderer` 
  together. You can use the `BMFFontLoader` separately if you want ot handly the vertices 
  in a different way. The `BMFFontRenderer` uses an optimized openGL render path using 
  multidraw arrays and a stream draw VBO (could be more optimized by mapping the VBO 
  memory to pinned space).


  - This class is to be used in a way where you "add" texts vertices to a buffer. You can 
  either add text once (when it's static), or update it for every frame. If your text changes
  you need to add it again and make sure to call `BMFFont::reset()`.

*/


#ifndef ROXLU_BMFONT_H
#define ROXLU_BMFONT_H

#include <vector>
#include <string>
#include <bmfont/BMFLoader.h>
#include <bmfont/BMFRenderer.h>

template<class T = BMFVertex>
class BMFFont {
 public:
  BMFFont();
  ~BMFFont();

  bool setup(std::string filename,                                                      /* sets up a font + renderer and shader. The pageSize is used by the VBO that contains the vertices for the text. When it needs to grow, it will grow `pageSize` or a multiple of this. */
             int windowW,  
             int windowH, 
             size_t pageSize = 1024, 
             bool datapath = false);     

  size_t allocate(int numPages);                                                        /* pre allocate some buffer to improve performance; make sure to only call this after calling `setup()` and only call it once! when pageSize = 1024 and you use numPages = 10, we callocate 10240 bytes */

  size_t addText(std::string str,                                                       /* add a new text and return indices to the offsets and vertex counts of the BMFRenderer, see BMFRenderer for more info */
                 float x,                                                               /* generate vertices add this x */
                 float y,                                                               /* generate vertices add this y */
                 float xchange = 0.0f,                                                  /* offset the vertex position with this xchange value */
                 float ychange = 0.0f);                                                 /* offset the vertex position with this ychange value; this can be used when you want to e.g. rotate text around the X-axis. If you want to rotate the vertices around the center of the text the vertices need to be generated "around" the X-axis. */

  void removeText(size_t index);                                                        /* remove the vertices for the given index, which was returned by `addText()` */
 
  void replaceText(size_t dx, std::string str, float x, float y, float xchange = 0.0f, float ychange = 0.0f);
  size_t addVertices(std::vector<T>& in);
  void replaceVertices(size_t index, std::vector<T>& vertices);                              /* replaces the text with some other vertices  */
  void setColor(float r, float g, float b, float a = 1.0);
  void setAlpha(float a);

 std::vector<T> generateVertices(std::string str, float x, float y, float xchange = 0.0f, float ychange = 0.0f);

  void flagChanged();                                                                   /* must be called if you want to upload the vertices to the gpu, when you add text we will automatically calls this for you */
  void update();                                                                        /* update will make sure that the vertices will be sent to the gpu whenever the've been changed. */
  void draw();
  void debugDraw();
  bool bind();                                                                          /* bind the GL objects used for drawing; only call this manually when you're using `drawText()`, when  you use only `draw()` don't call this. Only call bind() once per frame. */
  void drawText(size_t index);                                                          /* only draw a specific text entry */
  void drawText(size_t index, const float* modelMatrix);                                /* only draw a specific text entry + with the given model matrix */
  void reset();
  void print();
  void getStringSize(std::string str, int& w, int& h);                                  /* get the width and max height for the given string */
  size_t getNumVertices();                                                              /* get the number of vertices used to render the text*/
  void onResize(int w, int h);                                                         /* whenever the viewport resizes call this so we can update our projection matrix */
 
 public:
  BMFLoader<T> loader;
  BMFRenderer<T> renderer;
};

template<class T>
BMFFont<T>::BMFFont()
:renderer(loader)
{
}

template<class T>
BMFFont<T>::~BMFFont() {
}

template<class T>
inline bool BMFFont<T>::setup(std::string filename, int windowW, int windowH, size_t pageSize,bool datapath) {
  bool r = true;
  r = loader.load(filename, datapath);
  renderer.setup(windowW, windowH, pageSize);
  return r;
}

template<class T>
inline size_t BMFFont<T>::allocate(int numPages) {
  return renderer.allocate(numPages);
}

template<class T>
inline void BMFFont<T>::setColor(float r, float g, float b, float a) {
  loader.setColor(r, g, b, a);
}

template<class T>
inline std::vector<T> BMFFont<T>::generateVertices(std::string str, float x, float y, float xchange, float ychange) {
  return loader.generateVertices(str, x, y, xchange, ychange);
}

template<class T>
inline size_t BMFFont<T>::addVertices(std::vector<T>& in) {
  flagChanged(); // @todo check if this is necessary
  return renderer.addVertices(in);
}

template<class T>
inline size_t BMFFont<T>::addText(std::string str, float x, float y, float xchange, float ychange) {
  std::vector<T> vertices = loader.generateVertices(str, x, y, xchange, ychange);
  flagChanged();
  return renderer.addVertices(vertices);
}

template<class T>
inline void BMFFont<T>::replaceText(size_t dx, std::string str, float x, float y, float xchange, float ychange) {
  std::vector<T> vertices = loader.generateVertices(str, x, y, xchange, ychange);
  renderer.replaceVertices(dx, vertices);
}

template<class T>
inline void BMFFont<T>::replaceVertices(size_t dx, std::vector<T>& in) {
  renderer.replaceVertices(dx, in);
}


template<class T>
inline void BMFFont<T>::removeText(size_t index) { 
  renderer.removeVertices(index);
}

template<class T>
inline void BMFFont<T>::update() {
  renderer.update();
}

template<class T>
inline void BMFFont<T>::draw() {
  renderer.bind();
  renderer.update();
  renderer.draw();
}

template<class T>
inline void BMFFont<T>::debugDraw() {
  renderer.update();
  renderer.debugDraw();
}

template<class T>
inline void BMFFont<T>::drawText(size_t text) {
  renderer.drawText(text);
}

template<class T>
inline void BMFFont<T>::drawText(size_t text, const float* mm) {
  renderer.drawText(text, mm);
}

template<class T>
inline void BMFFont<T>::setAlpha(float a) {
  renderer.setAlpha(a);
}

template<class T>
inline void BMFFont<T>::print() {
  loader.print();
}

template<class T>
inline void BMFFont<T>::reset() {
  renderer.reset();
}

template<class T>
inline bool BMFFont<T>::bind() {
  return renderer.bind();
}

template<class T>
inline void BMFFont<T>::getStringSize(std::string str, int& w, int& h) {
  loader.getStringSize(str, w, h);
}

template<class T>
inline size_t BMFFont<T>::getNumVertices() {
  return renderer.size();
}

template<class T>
inline void BMFFont<T>::flagChanged() {
  return renderer.flagChanged();
}

template<class T>
inline void BMFFont<T>::onResize(int winW, int winH) {
  renderer.onResize(winW, winH);
}


#endif
