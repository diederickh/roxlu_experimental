#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <rapidxml/rapidxml.hpp>
#include <bmfont/BMFont.h>

using namespace rapidxml;

// -------------------------------------------

void bmfont_init_character(BVertex& a, BVertex& b, BVertex& c, BVertex& d, size_t dx, void* user) {

}

// -------------------------------------------

BMFontLoader::BMFontLoader() 
  :cb_user(this)
  ,cb_init_char(bmfont_init_character)
  ,image_width(0)
  ,image_height(0)
{
  color[0] = color[1] = color[2] = color[3] = 1.0f;
}


BMFontLoader::~BMFontLoader() {
  clear();
}

void BMFontLoader::clear() {
  image_width = 0;
  image_height = 0;
  image_path = "";

  chars.clear();

  cb_user = NULL;
  cb_init_char = NULL;
}

bool BMFontLoader::load(std::string filename, bool datapath) {
  std::string str = rx_get_file_contents(filename, datapath);
  if(!str.size()) {
    RX_ERROR((BMF_ERR_FILE));
    return false;
  }

  xml_document<> doc;

  try {
    doc.parse<0>((char*)str.c_str());
  }
  catch(std::exception& ex) {
    RX_ERROR((BMF_ERR_PARSE, ex.what()));
    return false;
  }
  
  xml_node<>* font_node = doc.first_node("font");
  xml_node<>* common_node = font_node->first_node("common");
  xml_node<>* page_node = font_node->first_node("pages")->first_node("page");
  xml_attribute<>* attr = NULL;
  
  if(!common_node) {
    RX_ERROR((BMF_ERR_NO_COMMON));
    return false;
  }

  // IMAGE WIDTH
  attr = common_node->first_attribute("scaleW");
  if(!attr) {
    RX_ERROR((BMF_ERR_NO_SCALEW));
    return false;
  }
  image_width = rx_string_to_int(attr->value());

  // IMAGE HEIGHT
  attr = common_node->first_attribute("scaleH");
  if(!attr) {
    RX_ERROR((BMF_ERR_NO_SCALEW));
    return false;
  }
  image_height = rx_string_to_int(attr->value());


  // IMAGE PATH
  if(!page_node) {
    RX_ERROR((BMF_ERR_NO_PAGE_NODE));
    return false;
  }

  attr = page_node->first_attribute("file");
  if(!attr) {
    RX_ERROR((BMF_ERR_NO_FILE_ATTR));
    return false;
  }

  image_path = attr->value();
  if(datapath) {
    image_path = rx_to_data_path(image_path);
  }

  // CHARS
  xml_node<>* node = font_node->first_node("chars")->first_node("char");
  do {

    attr = node->first_attribute();
    BChar bchar;

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

std::vector<BVertex> BMFontLoader::generateVertices(std::string str, float x, float y) {
  float xoffset = x;
  float yoffset = y;
  std::vector<BVertex> result;

  for(size_t i = 0; i < str.size(); ++i) {
    std::map<size_t, BChar>::iterator it = chars.find(str[i]);
    if(it == chars.end()) {
      RX_ERROR((BMF_ERR_CHAR_NOT_FOUND, str[i]));
      continue;
    }
    
    BChar& bchar = it->second;

    float x0 = xoffset + bchar.xoffset;
    float y0 = yoffset + bchar.yoffset;
    float x1 = x0 + bchar.width;
    float y1 = bchar.height + yoffset + bchar.yoffset;

    BVertex a(x0, y1, bchar.x, bchar.y + bchar.height);
    BVertex b(x1, y1, bchar.x + bchar.width, bchar.y + bchar.height);
    BVertex c(x1, y0, bchar.x + bchar.width, bchar.y);
    BVertex d(x0, y0, bchar.x, bchar.y);

    a.setColor4fv(color);
    b.setColor4fv(color);
    c.setColor4fv(color);
    d.setColor4fv(color);

    cb_init_char(a, b, c, d, i, cb_user);

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


void BMFontLoader::print() {
  RX_VERBOSE(("bm_font.image_width: %d", image_width));
  RX_VERBOSE(("bm_font.image_height: %d", image_height));
  RX_VERBOSE(("bm_font.image_path: %s", image_path.c_str()));
}
