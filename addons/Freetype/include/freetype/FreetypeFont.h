#ifndef ROXLU_FREETYPE_FONT_H
#define ROXLU_FREETYPE_FONT_H

#include <roxlu/core/Utils.h>
#include <string.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#define FF_ERR_CANNOT_INIT "Cannot initialize FreeType"
#define FF_ERR_NOT_INIT "Not initialzed"
#define FF_ERR_CANNOT_FREE "Cannot deinitialize/free FreeType"
#define FF_ERR_CANNOT_NEW_FACE "Cannot create a new FF_Face"
#define FF_ERR_CANNOT_SET_SIZE "Cannot set the font size"
#define FF_ERR_CANNOT_FIND_CHAR "Cannot load character"
#define FF_VERB_NUM_GLYPHS "face.num_glyphs: %ld"
#define FF_VERB_EM "face.units_per_EM: %d"
#define FF_VERB_FIXED_SIZES "face.num_fixed_sizes: %d"

enum FreetypeFontType {
  FF_GRAYSCALE,
  FF_RGB24,
  FF_RGBA32,
  FF_BGRA32, /* optimimal GL type with GL_UNSIGNED_INT_8_8_8_8_REV, see apple docs */
};

class FreetypeFont {
 public:
  FreetypeFont();
  ~FreetypeFont();

  bool setup(std::string filepath, unsigned int size,                                 /* setup freetype, load the file at `filepath`, with fontsize `size` in pixels */
              int surfaceW, int surfaceH,                                             /* ... the surface width/height into which we render */ 
             FreetypeFontType type = FF_GRAYSCALE,
              bool datapath = false                                                   /* ... load the font from the data path */
              );

  bool render(std::string str, int penX, int penY);                                   /* render a string with baseline x/y. NOTE: penX/penY are not the top left, but the bottom left of the base line */
  void clear();                                                                       /* clear the surface, you must do this each time you want to clear the surface // render a new string */
  void print();                                                                       /* prints some debug info */
  bool save(std::string filepath, bool datapath = false);   
  void setColor(float r, float g, float b, float a = 1.0);
  void fill(float r, float g, float b, float a = 1.0);

 private:
  bool copyGlyphPixels(unsigned char* dest, FT_Bitmap* bitmap, int x, int y);         /* internally used by render() to copy the rendered bitmap to the surface buffer */

 public:
  unsigned char* pixels;                                                              /* this will contains the rendered text as bitmap */
  FT_Library library;                                                                 /* the "freetype library", basically the context for FT */
  FT_Face face;                                                                       /* the loaded font face, for now it must have a unicode character map */

  int num_bytes;                                                                      /* the number of bytes in the pixel array, which is a grayscale image */
  int surface_w;                                                                      /* the width of the surface to which we write */
  int surface_h;                                                                      /* the height of the surface to which we write */
  int font_size;                                                                      /* the font size, given to load()  */
  bool is_initialized;                                                                /* internally used to check if freetype is initialized */
  FreetypeFontType type;
  int num_components;
  int surface_stride;
  float font_color[4];
  
  int min_x;                                                                          /* after calling render(), this is the min_x position in the surface that contains the rendered string */
  int max_x;                                                                          /* after calling render(), this is the max_x position in the surface that contains the rendered string */
  int min_y;                                                                          /* after calling render(), this is the min_y position in the surface that contains the rendered string */
  int max_y;                                                                          /* after calling render(), this is the max_y position in the surface that contains the rendered string */
  int rect_w;                                                                         /* after calling render(), this is set to the 'width' of the rendered rectangle. Can be used as stride into `pixels` */
  int rect_h;                                                                         /* after calling render(), this is set to the 'height' of the rendered rectangle. */
};

inline void FreetypeFont::setColor(float r, float g, float b, float a) {
  font_color[0] = r;
  font_color[1] = g;
  font_color[2] = b;
  font_color[3] = a;
  //printf("r: %f, g: %f, b: %f, a: %f\n", r,g,b,a);
}

#endif
