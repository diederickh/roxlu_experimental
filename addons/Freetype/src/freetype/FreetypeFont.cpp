#include <freetype/FreetypeFont.h>
#include <roxlu/experimental/Image.h>

FreetypeFont::FreetypeFont() 
  :is_initialized(false)
  ,surface_w(0)
  ,surface_h(0)
  ,font_size(0)
  ,num_bytes(0)
  ,min_x(0)
  ,max_x(0)
  ,min_y(0)
  ,max_y(0)
  ,rect_w(0)
  ,rect_h(0)
  ,face(NULL)
  ,library(NULL)
  ,type(FF_GRAYSCALE)
  ,num_components(0)
  ,surface_stride(0)
{
  if(FT_Init_FreeType(&library)) {
    RX_ERROR((FF_ERR_CANNOT_INIT));
  }

  is_initialized = true;
  setColor(1.0f, 1.0, 1.0, 1.0);
}

FreetypeFont::~FreetypeFont() {
  if(is_initialized) {
    if(FT_Done_FreeType(library)) {
      RX_ERROR((FF_ERR_CANNOT_FREE));
    }
  }

  if(pixels) {
    delete[] pixels;
    pixels = NULL;
  }

  surface_w = 0;
  surface_h = 0;
  surface_stride = 0;
  font_size = 0;
  num_bytes = 0;
  num_components = 0;
  
  min_x = 0;
  max_x = 0;
  min_y = 0;
  max_y = 0;
  rect_w = 0;
  rect_h = 0;
  library = NULL;
  face = NULL;
}

bool FreetypeFont::setup(std::string filename, unsigned int size, 
                         int surfaceW, int surfaceH,
                         FreetypeFontType ftype,  bool datapath)
{
  type = ftype;

  if(type == FF_GRAYSCALE) {
    num_components = 1;
  }
  else if(type == FF_RGB24) {
    num_components = 3;
  }
  else if(type == FF_RGBA32) {
    num_components = 4;
  }
  else if(type == FF_BGRA32) {
    num_components = 4;
  }

  surface_w = surfaceW;
  surface_h = surfaceH;
  surface_stride = num_components * surfaceW;
  num_bytes = surface_w * surface_h * num_components;
  font_size = size;

  min_x = surface_w;
  max_x = 0;
  min_y = surface_h;
  max_y = 0;
  rect_w = 0;
  rect_h = 0;

  if(!is_initialized) {
    RX_ERROR((FF_ERR_NOT_INIT));
    return false;
  }

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  if(FT_New_Face(library, filename.c_str(), 0, &face)) {
    RX_ERROR((FF_ERR_CANNOT_NEW_FACE));
    return false;
  }

  if(FT_Set_Pixel_Sizes(face, 0, size)) {
    RX_ERROR((FF_ERR_CANNOT_SET_SIZE));
    return false;
  }

  pixels = new unsigned char[num_bytes];
  clear();

  return true;
}

bool FreetypeFont::copyGlyphPixels(unsigned char* dest, FT_Bitmap* bitmap, int x, int y) {
  int dest_dx = 0;
  int src_dx = 0;

  unsigned char* dest_ptr = dest;
  unsigned char* src_ptr = bitmap->buffer;

  if(x < 0) {
    x = 0;
  }

  if(x < min_x) {
    min_x = x;
  }

  if( (x + bitmap->width) > max_x) {
    max_x = x + bitmap->width;
  }
  
  int start_y = font_size + y;
  if(start_y < min_y) {
    min_y = start_y;
  }

  if((start_y + bitmap->rows) > max_y) {
    max_y = start_y + bitmap->rows;
  }

  if(type == FF_GRAYSCALE) { 
    for(int j = 0; j < bitmap->rows; ++j) {
      dest_dx = (start_y + j) * surface_stride + x;
      src_dx = j * bitmap->width;
      memcpy(dest_ptr + dest_dx, src_ptr + src_dx, bitmap->pitch);
    }
  }
  else if(type == FF_RGBA32) {
    int dest_row_dx = 0;
    unsigned char src_value;

    for(int j = 0; j < bitmap->rows; ++j) {
      dest_row_dx = (start_y + j) * surface_stride + (x * num_components);
      for(int i = 0; i < bitmap->width; ++i) {
        dest_dx = dest_row_dx + (i * num_components);
        src_dx = (j * bitmap->width) + i;
        src_value = *(src_ptr + src_dx);
       
        *(dest_ptr + dest_dx + 0) |= (unsigned char)(src_value * font_color[0]);
        *(dest_ptr + dest_dx + 1) |= (unsigned char)(src_value * font_color[1]);
        *(dest_ptr + dest_dx + 2) |= (unsigned char)(src_value * font_color[2]);
        // *(dest_ptr + dest_dx + 3) |= (unsigned char)(255 * font_color[3]);
      }
    }
  }
  else if(type == FF_BGRA32) {
    int dest_row_dx = 0;
    unsigned char src_value;

    for(int j = 0; j < bitmap->rows; ++j) {
      dest_row_dx = (start_y + j) * surface_stride + (x * num_components);
      for(int i = 0; i < bitmap->width; ++i) {
        dest_dx = dest_row_dx + (i * num_components);
        src_dx = (j * bitmap->width) + i;
        src_value = *(src_ptr + src_dx);

        /*
        dest_ptr[dest_dx + 0] = (dest_ptr[dest_dx + 0] + (unsigned char)(src_value * font_color[2])) * 0.5;
        dest_ptr[dest_dx + 1] = (dest_ptr[dest_dx + 1] + (unsigned char)(src_value * font_color[1])) * 0.5;
        dest_ptr[dest_dx + 2] = (dest_ptr[dest_dx + 2] + (unsigned char)(src_value * font_color[0])) * 0.5;
        */
        /*
        dest_ptr[dest_dx + 0] = std::min<unsigned char>(255,  (unsigned char)(src_value * font_color[2]));
        dest_ptr[dest_dx + 1] = std::min<unsigned char>(255,  (unsigned char)(src_value * font_color[1]));
        dest_ptr[dest_dx + 2] = std::min<unsigned char>(255,  (unsigned char)(src_value * font_color[0]));
        dest_ptr[dest_dx + 3] = 127; // src_value; // * font_color[3];
        */

        
        dest_ptr[dest_dx + 0] = font_color[2] * src_value; 
        dest_ptr[dest_dx + 1] = font_color[1] * src_value;
        dest_ptr[dest_dx + 2] = font_color[0] * src_value;
        dest_ptr[dest_dx + 3] = src_value;



        /*
        unsigned char src_r = src_value * font_color[0];
        unsigned char src_g = src_value * font_color[1];
        unsigned char src_b = src_value * font_color[2];
        unsigned char src_a = src_value * font_color[3];

        unsigned char dest_r = dest_ptr[dest_dx + 2];
        unsigned char dest_g = dest_ptr[dest_dx + 1];
        unsigned char dest_b = dest_ptr[dest_dx + 0];
        unsigned char dest_a = dest_ptr[dest_dx + 3];

        dest_ptr[dest_dx + 2] = (dest_r + ((src_r - dest_r) * src_a) / 255.0f);
        dest_ptr[dest_dx + 1] = (dest_g + ((src_g - dest_g) * src_a) / 255.0f);
        dest_ptr[dest_dx + 0] = (dest_b + ((src_b - dest_b) * src_a) / 255.0f);
        dest_ptr[dest_dx + 3] = std::min<int>(255, src_a +  dest_a);
        */

        //        dest_ptr[dest_dx + 1] = (dest_ptr[dest_dx + 1] + (unsigned char)(src_value * font_color[1])) * 0.5;
        //        dest_ptr[dest_dx + 2] = (dest_ptr[dest_dx + 2] + (unsigned char)(src_value * font_color[0])) * 0.5;

        //     dest_ptr[dest_dx + 3] = (dest_ptr[dest_dx + 3] + (unsigned char)(src_value * font_color[3])) * 0.5;

        /*
        *(dest_ptr + dest_dx + 1) |= (unsigned char)(src_value * font_color[1]);
        *(dest_ptr + dest_dx + 2) |= (unsigned char)(src_value * font_color[0]);
        *(dest_ptr + dest_dx + 3) = 255; // (unsigned char)(src_value * font_color[0]);
        */
        //        *(dest_ptr + dest_dx + 3) =  (unsigned char)(src_value * font_color[0]);
          //          *(dest_ptr + dest_dx + 3) *= (unsigned char)(src_value * font_color[3]);

 
        // *(dest_ptr + dest_dx + 3) = 0;
      }
    }
  }
  else if(type == FF_RGB24) {
    int dest_row_dx = 0;
    unsigned char src_value;

    for(int j = 0; j < bitmap->rows; ++j) {
      dest_row_dx = (start_y + j) * surface_stride + (x * num_components);
      for(int i = 0; i < bitmap->width; ++i) {
        dest_dx = dest_row_dx + (i * num_components);
        src_dx = (j * bitmap->width) + i;
        src_value = *(src_ptr + src_dx);
       
        *(dest_ptr + dest_dx + 0) |= (unsigned char)(src_value * font_color[0]);
        *(dest_ptr + dest_dx + 1) |= (unsigned char)(src_value * font_color[1]);
        *(dest_ptr + dest_dx + 2) |= (unsigned char)(src_value * font_color[2]);
      }
    }
  }
  return true;
}

void FreetypeFont::clear() {
  if(pixels) {
    memset(pixels, 0x00, num_bytes);
  }
}

bool FreetypeFont::render(std::string str, int penX, int penY) {
  int pen_x = penX;
  int pen_y = penY;
  FT_GlyphSlot slot = face->glyph;

  for(int i = 0; i < str.size(); ++i) {
    if(FT_Load_Char(face, str[i], FT_LOAD_RENDER)) {
      RX_ERROR((FF_ERR_CANNOT_FIND_CHAR));
      continue;
    }
    copyGlyphPixels(pixels, &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top);
    pen_x += slot->advance.x >> 6;
  }

  rect_w = max_x - min_x;
  rect_h = max_y - min_y;
  
  return true;
}

void FreetypeFont::fill(float rr, float gg, float bb, float aa) {
  unsigned char r = 255 * rr;
  unsigned char g = 255 * gg;
  unsigned char b = 255 * bb;
  unsigned char a = 255 * aa;


  if(type == FF_GRAYSCALE) {
    memset(pixels, r, num_bytes);
  }
  else if(type == FF_RGB24) {
    unsigned char* row_cols = new unsigned char[surface_stride];
    int dx = 0;
    for(int i = 0; i < surface_w; ++i) {
      dx = i * num_components;
      row_cols[dx + 0] = r;
      row_cols[dx + 1] = g;
      row_cols[dx + 2] = b;
    }

    for(int j = 0; j < surface_h; ++j) {
      dx = j * surface_stride;
      memcpy(pixels+dx, row_cols, surface_stride);
    }

    delete[] row_cols;
  }
  else if(type == FF_BGRA32) {
    unsigned char* row_cols = new unsigned char[surface_stride];
    int dx = 0;
    for(int i = 0; i < surface_w; ++i) {
      dx = i * num_components;
      row_cols[dx + 0] = b;
      row_cols[dx + 1] = g;
      row_cols[dx + 2] = r;
      row_cols[dx + 3] = a;
    }

    for(int j = 0; j < surface_h; ++j) {
      dx = j * surface_stride;
      memcpy(pixels+dx, row_cols, surface_stride);
    }

    delete[] row_cols;
  }
  else if(type == FF_RGBA32) {
    unsigned char* row_cols = new unsigned char[surface_stride];
    int dx = 0;
    for(int i = 0; i < surface_w; ++i) {
      dx = i * num_components;
      row_cols[dx + 0] = r;
      row_cols[dx + 1] = g;
      row_cols[dx + 2] = b;
      row_cols[dx + 3] = a;
    }

    for(int j = 0; j < surface_h; ++j) {
      dx = j * surface_stride;
      memcpy(pixels+dx, row_cols, surface_stride);
    }

    delete[] row_cols;
  }

}

void FreetypeFont::print() {
  RX_VERBOSE(FF_VERB_NUM_GLYPHS, face->num_glyphs);
  RX_VERBOSE(FF_VERB_EM, face->units_per_EM);
  RX_VERBOSE(FF_VERB_FIXED_SIZES, face->num_fixed_sizes);
}

bool FreetypeFont::save(std::string filename, bool datapath) {
  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  roxlu::Image img;
  img.copyPixels(pixels, surface_w, surface_h, num_components);
  return img.save(filename);
}
