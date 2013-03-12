#ifndef ROXLU_BMFONT_LOADER_H
#define ROXLU_BMFONT_LOADER_H

#include <string>
#include <vector>
#include <map>
#include <bmfont/BMTypes.h>
#include <bmfont/BMFontRenderer.h>

#define BMF_ERR_FILE "Error while loading the file from disk. Is it empty? Does it exist?"
#define BMF_ERR_PARSE "Parse error: %s"
#define BMF_ERR_CHAR_NOT_FOUND "Character not found: %c"
#define BMF_ERR_NO_COMMON "No <common> attribute fount in font description"
#define BMF_ERR_NO_SCALEW "No scaleW found in <common>"
#define BMF_ERR_NO_SCALEH "No scaleH found in <common>"
#define BMF_ERR_NO_PAGE_NODE "No <pages><page></page></pages> node found"
#define BMF_ERR_NO_FILE_ATTR "<page> does not contain a file attribute"

typedef void (*bmfont_init_character_callback)(BVertex& a,                      /* Callback that will be called for each character for which we create vertices.  */
                                               BVertex& b,                      /* We pass 4 vertices into this function, in this order:  */
                                               BVertex& c,                      /* bottom left, bottom right, top right, top left */
                                               BVertex& d, 
                                               size_t dx,                       /* `dx` is the position of the char in the given string */
                                               void* user);

extern void bmfont_init_character(BVertex& a, BVertex& b, BVertex& c, BVertex& d, size_t dx, void* user);

class BMFontLoader {
 public:
  BMFontLoader();
  ~BMFontLoader();
  bool load(std::string file, bool datapath = false);                            /* load a BMFont .xml type file */
  void setColor(float r, float g, float b, float a = 1.0);                       /* set the color for new vertices */
  void setCallback(bmfont_init_character_callback initCB, void* userCB);         /* set the callback that will be called for each character for which we create vertices */
  std::vector<BVertex> generateVertices(std::string str, float x, float y);      /* generate vertices for the given string at position x/y */
  void print();                                                                  /* print some debug info */

  std::string getImagePath();                                                    /* get the path of the image that is used by this the BMFont renderer. The image is stored in the xml file. */
  int getImageWidth();                                                           /* get the image width as defined in the xml file */ 
  int getImageHeight();                                                          /* get the image height ad defined in the xml file */

 private:
  void clear();                                                                  /* resets state; deallocs all allocated data */

 private:
  float color[4];                                                                /* the color that will be given to each of the vertices by default */
  std::map<size_t, BChar> chars;                                                 /* this map stores all information about the characters (offsets, xadvance, texcoords, etc.. */
  std::string image_path;                                                        /* path to image that must be used as texture for the font */
  int image_width;                                                               /* bmfont texture width */
  int image_height;                                                              /* bmfont texture height */
  bmfont_init_character_callback cb_init_char;                                   /* this function will be called for each character for which we create vertices */
  void* cb_user;                                                                 /* passed as data to the callback */
};

inline int BMFontLoader::getImageWidth() {
  return image_width;
}

inline int BMFontLoader::getImageHeight() {
  return image_height;
}

inline std::string BMFontLoader::getImagePath() {
  return image_path;
}

inline void BMFontLoader::setColor(float r, float g, float b, float a) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}

inline void BMFontLoader::setCallback(bmfont_init_character_callback initCB, void* userCB) {
  cb_init_char = initCB;
  cb_user = userCB;
}
#endif
