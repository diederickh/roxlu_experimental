#ifndef ROXLU_TEXT_SURFACEH
#define ROXLU_TEXT_SURFACEH

// $(ROXLU_OF_DIR)\libs\openFrameworks;$(ROXLU_OF_DIR)\libs\openFrameworks\graphics;$(ROXLU_OF_DIR)\libs\openFrameworks\app;$(ROXLU_OF_DIR)\libs\openFrameworks\sound;$(ROXLU_OF_DIR)\libs\openFrameworks\utils;$(ROXLU_OF_DIR)\libs\openFrameworks\communication;$(ROXLU_OF_DIR)\libs\openFrameworks\video;$(ROXLU_OF_DIR)\libs\openFrameworks\types;$(ROXLU_OF_DIR)\libs\openFrameworks\math;$(ROXLU_OF_DIR)\libs\openFrameworks\3d;$(ROXLU_OF_DIR)\libs\openFrameworks\gl;$(ROXLU_OF_DIR)\libs\openFrameworks\events;$(ROXLU_OF_DIR)\libs\glut\include;$(ROXLU_OF_DIR)\libs\rtAudio\include;$(ROXLU_OF_DIR)\libs\quicktime\include;$(ROXLU_OF_DIR)\libs\freetype\include;$(ROXLU_OF_DIR)\libs\freetype\include\freetype2;$(ROXLU_OF_DIR)\libs\freeImage\include;$(ROXLU_OF_DIR)\libs\fmodex\include;$(ROXLU_OF_DIR)\libs\videoInput\include;$(ROXLU_OF_DIR)\libs\glew\include\;$(ROXLU_OF_DIR)\libs\glu\include;$(ROXLU_OF_DIR)\libs\tess2\include;$(ROXLU_OF_DIR)\libs\cairo\include\cairo;$(ROXLU_OF_DIR)\libs\poco\include;$(ROXLU_OF_DIR)\addons;$(ROXLU_DIR)\extern\include\;$(ROXLU_DIR)\lib\include\;$(ROXLU_EXTERN_LIB_DIR)\msinttypes\;%(AdditionalIncludeDirectories)

#include <assert.h>
#include <string>
#include <pango/pangocairo.h>

class TextSurface {
 public:
  TextSurface();
  ~TextSurface();
  bool setup(const std::string& font, int w, int h, cairo_format_t fmt = CAIRO_FORMAT_ARGB32); //CAIRO_FORMAT_RGB24
  void setText(const std::string& txt, float r = 0.0f, float g = 0.0, float b = 0.0, float a = 1.0);
  void setMarkup(const std::string& txt);
  void setTextWidth(int w);
  void setTextHeight(int h);
  bool setFont(const std::string& font);
  void render();
  void fill(float r, float g, float b, float a = 1.0);
  void clear(); /* clears the surface */
  bool saveAsPNG(const std::string& filepath);
  PangoRectangle getPixelExtents(); /* get int (x,y,width,height) */
  void move(double x, double y);
  void alignLeft(); 
  void alignRight();
  void alignCenter();
  void wrapWord(); /* wrap on word boundaries */
  void wrapChar(); /* wrap on character boundaries */
  void wrapWordChar();  /* wrap on word boundaries, but fallback on char boundaries */
  unsigned char* getPixels(); 
  int getNumBytes(); /* get the number of bytes in the pixel array */
  //  unsigned char* getPixelsForPixelExtends(PangoRectangle rect); /* get only the pixels for the given area: IMPORTANT we allocate but YOU need to delete[] the allocated pixels! */
  int getWidth();
  int getHeight();
 public:
  cairo_t* cr;
  cairo_surface_t* surface;
  cairo_status_t status;
  PangoLayout* layout;
  PangoFontDescription* font_desc;
  int width;
  int height;
  static bool is_init;
};

inline void TextSurface::alignLeft() {
  assert(layout);
  pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
}

inline void TextSurface::alignRight() {
  assert(layout);
  pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
}

inline void TextSurface::alignCenter() {
  assert(layout);
  pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
}

inline void TextSurface::setTextWidth(int w) {
  assert(layout);
  pango_layout_set_width(layout, w * PANGO_SCALE);
}

inline void TextSurface::setTextHeight(int h) {
  assert(layout);
  pango_layout_set_height(layout, h * PANGO_SCALE);
}

inline void TextSurface::wrapWord() {
  assert(layout);
  pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
}

inline void TextSurface::wrapChar() {
  assert(layout);
  pango_layout_set_wrap(layout, PANGO_WRAP_CHAR);
}

inline void TextSurface::wrapWordChar() {
  assert(layout);
  pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
}

// PangoRectangle has: x, y, width, height (all ints);
// The logical extents (l) gives a bigger area then the ink extent; when using the ink extents (r), some chars didn't fit in the returned rectangle.
inline PangoRectangle TextSurface::getPixelExtents() { 
  assert(layout);
  PangoRectangle r;
  PangoRectangle l;
  pango_layout_get_pixel_extents(layout, &r, &l);
  return r;
}

inline void TextSurface::fill(float r, float g, float b, float a) {
  cairo_save(cr);
   cairo_set_source_rgba(cr, r,g,b,a);
  // cairo_set_source_rgba(cr, g,b,a,r);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  cairo_restore(cr);
}

inline void TextSurface::clear() {
  fill(0.0, 0.0, 0.0, 0.0);
}

inline int TextSurface::getWidth() {
  return width;
}

inline int TextSurface::getHeight() {
  return height;
}

inline int TextSurface::getNumBytes() {
  cairo_format_t fmt = cairo_image_surface_get_format(surface);
  if(fmt == CAIRO_FORMAT_RGB24) {
    return width * height * 3;
  }
  else if(fmt == CAIRO_FORMAT_ARGB32) {
    return width * height * 4;
  }
  else if(fmt == CAIRO_FORMAT_A8) {
    return width * height;
  }
  else {
   printf("cannot calculate num of bytes, need to add this format\n");
  }

  return 0;
}
#endif
