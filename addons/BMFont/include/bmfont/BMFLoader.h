/*
  BMFLoader
  ---------
  The BMFLoader is a pure BMFont parser. It loads the XML file and generates
  vertices for a given string. This class doesn't do any rendering. For 
  rendering see BMFRenderer

  The BMF* types are templated on the `vertex` type. See BMFTypes.h for an example
  how you vertex should look like, if  you want to use your own vertex types.

 */

#ifndef ROXLU_BMFONT_LOADER_H
#define ROXLU_BMFONT_LOADER_H

#include <string>
#include <vector>
#include <map>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <rapidxml/rapidxml.hpp>
#include <bmfont/BMFTypes.h>
#include <bmfont/BMFRenderer.h>

using namespace rapidxml;

#define BMF_ERR_FILE "Error while loading the file from disk. Is it empty? Does it exist?"
#define BMF_ERR_PARSE "Parse error: %s"
#define BMF_ERR_CHAR_NOT_FOUND "Character not found: %c"
#define BMF_ERR_NO_COMMON "No <common> attribute fount in font description"
#define BMF_ERR_NO_SCALEW "No scaleW found in <common>"
#define BMF_ERR_NO_SCALEH "No scaleH found in <common>"
#define BMF_ERR_NO_PAGE_NODE "No <pages><page></page></pages> node found"
#define BMF_ERR_NO_FILE_ATTR "<page> does not contain a file attribute"


template<class T>
class BMFLoader {
 public:
  BMFLoader();
  ~BMFLoader();
  bool load(std::string file, bool datapath = false);                            /* load a BMFont .xml type file */
  void setColor(float r, float g, float b, float a = 1.0);                       /* set the color for new vertices */
  std::vector<T> generateVertices(std::string str, float x, float y);            /* generate vertices for the given string at position x/y */
  void print();                                                                  /* print some debug info */

  std::string getImagePath();                                                    /* get the path of the image that is used by this the BMFont renderer. The image is stored in the xml file. */
  int getImageWidth();                                                           /* get the image width as defined in the xml file */ 
  int getImageHeight();                                                          /* get the image height ad defined in the xml file */
  void getStringSize(std::string& str, int& w, int &h);                          /* get the max width and max height for the given string */

 private:
  void clear();                                                                  /* resets state; deallocs all allocated data */

 private:
  float color[4];                                                                /* the color that will be given to each of the vertices by default */
  std::map<size_t, BMFChar> chars;                                               /* this map stores all information about the characters (offsets, xadvance, texcoords, etc.. */
  std::string image_path;                                                        /* path to image that must be used as texture for the font */
  int image_width;                                                               /* bmfont texture width */
  int image_height;                                                              /* bmfont texture height */
};

template<class T>
inline int BMFLoader<T>::getImageWidth() {
  return image_width;
}

template<class T>
inline int BMFLoader<T>::getImageHeight() {
  return image_height;
}

template<class T>
inline std::string BMFLoader<T>::getImagePath() {
  return image_path;
}

template<class T>
inline void BMFLoader<T>::setColor(float r, float g, float b, float a) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;
}

// -------------------------------------------
template<class T>
BMFLoader<T>::BMFLoader() 
  :image_width(0)
  ,image_height(0)
{
  color[0] = color[1] = color[2] = color[3] = 1.0f;
}


template<class T>
BMFLoader<T>::~BMFLoader() {
  clear();
}

template<class T>
void BMFLoader<T>::clear() {
  image_width = 0;
  image_height = 0;
  image_path = "";

  chars.clear();
}


template<class T>
bool BMFLoader<T>::load(std::string filename, bool datapath) {
  std::string str = rx_get_file_contents(filename, datapath);
  if(!str.size()) {
    RX_ERROR(BMF_ERR_FILE);
    return false;
  }

  xml_document<> doc;

  try {
    doc.parse<0>((char*)str.c_str());
  }
  catch(std::exception& ex) {
    RX_ERROR(BMF_ERR_PARSE, ex.what());
    return false;
  }
  
  xml_node<>* font_node = doc.first_node("font");
  xml_node<>* common_node = font_node->first_node("common");
  xml_node<>* page_node = font_node->first_node("pages")->first_node("page");
  xml_attribute<>* attr = NULL;
  
  if(!common_node) {
    RX_ERROR(BMF_ERR_NO_COMMON);
    return false;
  }

  // IMAGE WIDTH
  attr = common_node->first_attribute("scaleW");
  if(!attr) {
    RX_ERROR(BMF_ERR_NO_SCALEW);
    return false;
  }
  image_width = rx_string_to_int(attr->value());

  // IMAGE HEIGHT
  attr = common_node->first_attribute("scaleH");
  if(!attr) {
    RX_ERROR(BMF_ERR_NO_SCALEW);
    return false;
  }
  image_height = rx_string_to_int(attr->value());


  // IMAGE PATH
  if(!page_node) {
    RX_ERROR(BMF_ERR_NO_PAGE_NODE);
    return false;
  }

  attr = page_node->first_attribute("file");
  if(!attr) {
    RX_ERROR(BMF_ERR_NO_FILE_ATTR);
    return false;
  }

  image_path = attr->value();

  if(datapath) {
    std::string xml_path = rx_to_data_path(filename);
    std::string base_dir = rx_strip_filename(xml_path);
    image_path = base_dir + image_path;
  }

  // CHARS
  xml_node<>* node = font_node->first_node("chars")->first_node("char");
  do {

    attr = node->first_attribute();
    BMFChar bchar;

    do {
      std::string name = attr->name();

      if(name == "id") {
        bchar.id = rx_string_to_sizet(attr->value());
      }
      else if(name == "x") {
        bchar.x = rx_string_to_int(attr->value());
      }
      else if(name == "y") {
        bchar.y = rx_string_to_int(attr->value());
      }
      else if(name == "width") {
        bchar.width = rx_string_to_int(attr->value());
      }
      else if(name == "height") {
        bchar.height = rx_string_to_int(attr->value());
      }
      else if(name == "xoffset") {
        bchar.xoffset = rx_string_to_int(attr->value());
      }
      else if(name == "yoffset") {
        bchar.yoffset = rx_string_to_int(attr->value());
      }
      else if(name == "xadvance") {
        bchar.xadvance = rx_string_to_int(attr->value());
      }

    }
    while( (attr = attr->next_attribute()) );

    chars[bchar.id] = bchar;    

  } while( (node = node->next_sibling()) );

  return true;
}

template<class T>
std::vector<T> BMFLoader<T>::generateVertices(std::string str, float x, float y) {
  float xoffset = x;
  float yoffset = y;
  std::vector<T> result;

  for(size_t i = 0; i < str.size(); ++i) {
    std::map<size_t, BMFChar>::iterator it = chars.find(str[i]);
    if(it == chars.end()) {
      RX_ERROR(BMF_ERR_CHAR_NOT_FOUND, str[i]);
      continue;
    }
    
    BMFChar& bchar = it->second;

    float x0 = xoffset + bchar.xoffset;
    float y0 = yoffset + bchar.yoffset;
    float x1 = x0 + bchar.width;
    float y1 = bchar.height + yoffset + bchar.yoffset;

    T a(x0, y1, bchar.x, bchar.y + bchar.height);
    T b(x1, y1, bchar.x + bchar.width, bchar.y + bchar.height);
    T c(x1, y0, bchar.x + bchar.width, bchar.y);
    T d(x0, y0, bchar.x, bchar.y);

    a.setColor4fv(color);
    b.setColor4fv(color);
    c.setColor4fv(color);
    d.setColor4fv(color);

    result.push_back(a);
    result.push_back(b);
    result.push_back(c);

    result.push_back(a);
    result.push_back(c);
    result.push_back(d);

    xoffset += bchar.xadvance;
    
  }
  return result;
}

template<class T>
void BMFLoader<T>::getStringSize(std::string& str, int& w, int &h) {
  w = 0;
  h = 0;

  for(size_t i = 0; i < str.size(); ++i) {
    std::map<size_t, BMFChar>::iterator it = chars.find(str[i]);
    if(it == chars.end()) {
      RX_ERROR(BMF_ERR_CHAR_NOT_FOUND, str[i]);
      continue;
    }
    
    BMFChar& bchar = it->second;
    w += bchar.xoffset + bchar.xadvance;

    if(bchar.height > h) {
      h = bchar.height;
    }
  }
  
}


template<class T>
void BMFLoader<T>::print() {
  RX_VERBOSE("bm_font.image_width: %d", image_width);
  RX_VERBOSE("bm_font.image_height: %d", image_height);;
  RX_VERBOSE("bm_font.image_path: %s", image_path.c_str());
}


#endif
