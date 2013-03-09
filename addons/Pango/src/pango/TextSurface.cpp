#include <pango/TextSurface.h>
#include <pango/pangoft2.h>

bool TextSurface::is_init = false;

TextSurface::TextSurface() 
  :cr(NULL)
  ,surface(NULL)
  ,layout(NULL)
  ,font_desc(NULL)
  ,width(0)
  ,height(0)
{
  if(!is_init) {
    g_type_init();
    is_init = true;
  }
}

TextSurface::~TextSurface() {
  if(layout != NULL) {
    g_object_unref(layout);
    layout = NULL;
  }
}

bool TextSurface::setup(const std::string& font, int w, int h, cairo_format_t fmt) {
  width = w;
  height = h;

  surface = cairo_image_surface_create(fmt, width, height);
  if(surface == NULL) {
    printf("ERROR: Cannot create surface (%d, %d)\n", width, height);
    return false;
  }

  cr = cairo_create(surface);
  if(cairo_status(cr) == CAIRO_STATUS_NO_MEMORY) {
    printf("ERROR: Cannot create cairo context.\n");
    return false;
  }

  //cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);

  layout = pango_cairo_create_layout(cr);
  if(layout == NULL) {
    printf("ERROR: Cannot pango_cairo_create_layout()\n");
    return false;
  }

  // -- test
  /*
  PangoContext* c = pango_layout_get_context(layout);
  cairo_font_options_t* fo = cairo_font_options_create();
  cairo_font_options_set_antialias(fo, CAIRO_ANTIALIAS_GRAY);
  pango_cairo_context_set_font_options(c, fo);
  //const cairo_font_options_t* fo = pango_cairo_context_get_font_options(c);
  printf("%p ==\n", fo);
  //  cairo_font_options_set_antialias
  //pango_cairo_context_set_resolution(c, 16);
  */
  // -- end test

  font_desc = pango_font_description_from_string(font.c_str());
  if(font_desc == NULL) {
    printf("ERROR: Cannot pango_font_description_from_string(%s).\n", font.c_str());
    return false;
  }

  pango_layout_set_font_description(layout, font_desc);
  pango_font_description_free(font_desc);

  //mamove(0,0);
  setTextWidth(w);
  setTextHeight(h);

  font_desc = NULL;
  return true;
}

bool TextSurface::setFont(const std::string& font) {
  assert(layout);
  font_desc = pango_font_description_from_string(font.c_str());
  if(font_desc == NULL) {
    printf("ERROR: Cannot pango_font_description_from_string(%s).\n", font.c_str());
    return false;
  }

  pango_layout_set_font_description(layout, font_desc);
  pango_font_description_free(font_desc);
  font_desc = NULL;
  return true;
}

void TextSurface::setText(const std::string& txt, float r, float g, float b, float a) {
  assert(cr);
  assert(layout);
  cairo_set_source_rgba(cr, r, g, b, a);
  pango_layout_set_text(layout, txt.c_str(), -1);
}

void TextSurface::setMarkup(const std::string& txt) {
  assert(layout);
  pango_layout_set_markup(layout, txt.c_str(), txt.size());
}

void TextSurface::render() {
  assert(layout); 
  assert(cr);
  pango_cairo_show_layout(cr, layout);
}

bool TextSurface::saveAsPNG(const std::string& filepath) {
  assert(surface);
  status = cairo_surface_write_to_png(surface, filepath.c_str());
  if(status != CAIRO_STATUS_SUCCESS) {
    printf("ERROR: Cannot saveAsPNG(), to %s - %s\n", filepath.c_str(), cairo_status_to_string(status));
    return false;
  }
  return true;
}

void TextSurface::move(double x, double y) {
  assert(cr);
  cairo_move_to(cr, x, y);
}

unsigned char* TextSurface::getPixels() {
  cairo_surface_flush(surface);
  return cairo_image_surface_get_data(surface);
}

