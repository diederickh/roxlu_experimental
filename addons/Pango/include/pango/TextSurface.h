#ifndef ROXLU_TEXT_SURFACEH
#define ROXLU_TEXT_SURFACEH

#include <assert.h>
#include <string>
#include <pango/pangocairo.h>

class TextSurface {
 public:
  TextSurface();
  ~TextSurface();
  bool setup(const std::string& font, int w, int h, cairo_format_t fmt = CAIRO_FORMAT_ARGB32);
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
inline PangoRectangle TextSurface::getPixelExtents() { 
  assert(layout);
  PangoRectangle r;
  pango_layout_get_pixel_extents(layout, &r, NULL);
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
#endif
