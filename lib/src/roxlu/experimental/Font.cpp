#include <roxlu/experimental/Font.h>

namespace roxlu {
  // -----------------------------------------------------------------------------
  // CHAR VERTEX
  // -----------------------------------------------------------------------------
  CharVertex::CharVertex()
  {
    pos[0] = pos[1] = 0.0f;
    tex[0] = tex[1] = 0.0f;
    col[0] = col[1] = col[2] = col[3] = 1.0f;
  }

  CharVertex::CharVertex(float x, float y, float s, float t, const float* txtCol) { // , const float* bgCol) {
    pos[0] = x;
    pos[1] = y;
    tex[0] = s;
    tex[1] = t;
    bg[0] = bg[1] = bg[2] = bg[3] = 0.0f;
    memcpy(col, txtCol, sizeof(float) * 4);
  }

  // -----------------------------------------------------------------------------
  // FONT ENTRY
  // -----------------------------------------------------------------------------
  FontEntry::FontEntry()
    :start_dx(0)
    ,num_vertices(0)
    ,w(0)
    ,align(FEA_LEFT)
    ,right_edge(0)
    ,needs_update(false)
  {
    pos[0] = pos[1] = 0.0f;
  }


  // -----------------------------------------------------------------------------
  // FONT
  // -----------------------------------------------------------------------------
  Font::Font() 
    :w(512)
    ,h(512)
    ,cdata(NULL)
    ,allocated_bytes(0)
    ,is_initialized(false)
    ,tex(0)
    ,win_w(0)
    ,win_h(0)
    ,size(16)
    ,needs_update(false)
  {
    memset(pm, 0, sizeof(float) * 16);
    memset(mm, 0, sizeof(float) * 16);
    mm[0] = mm[5] = mm[10] = mm[15] = 1.0f;
  }

  Font::~Font() {
    if(cdata != NULL) {
      delete[] cdata;
      cdata = NULL;
    }
  }

  void Font::createOrtho(int winWidth, int winHeight) {
    float n = 0.0;
    float f = 1.0;
    float ww = winWidth;
    float hh = winHeight;
    float fmn = f - n;
    pm[1]  = 0.0f;
    pm[2]  = 0.0f;
    pm[3]  = 0.0f;
    pm[4]  = 0.0f;
    pm[6]  = 0.0f;
    pm[7]  = 0.0f;
    pm[8]  = 0.0f;
    pm[9]  = 0.0f;
    pm[11] = 0.0f;
    pm[15] = 1.0f;
    pm[0]  = 2.0f / ww;
    pm[5]  = 2.0f / -hh;
    pm[10] = -2.0f / fmn;
    pm[12] = -(ww)/ww;
    pm[13] = -(hh)/-hh;
    pm[14] = -(f+n)/fmn;
  }


  void Font::setupGL(unsigned char* bakedBitmap, int bw, int bh) {
    is_initialized = true;
 
    // TEXTURE
    // -------
    glGenTextures(1, &tex); eglGetError();
    glBindTexture(GL_TEXTURE_2D, tex); eglGetError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); eglGetError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); eglGetError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); eglGetError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); eglGetError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bw, bh, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bakedBitmap); eglGetError();

    // SHADER 
    // ------
    shader.create(FONT_VS, FONT_FS);
    shader.a("a_pos",0).a("a_tex", 1).a("a_col", 2).a("a_bg", 3);
    shader.link();
    shader.u("u_projection_matrix").u("u_model_matrix").u("u_font_texture");

    // BUFFERS
    vao.bind();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
  
    glEnableVertexAttribArray(0); // a_pos
    glEnableVertexAttribArray(1); // a_tex
    glEnableVertexAttribArray(2); // a_col
    glEnableVertexAttribArray(3); // a_bg
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, tex));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, col));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, bg));
  }

#define CHAR_TRI(a,b,c) { vertices.push_back(a); vertices.push_back(b); vertices.push_back(c); }
  int Font::write(const std::string& text, float x, float y, float r, float g, float b, float a) {
    needs_update = true;
    FontEntry e;
    float col[4] = { r, g, b, a };
    e.pos[0] = x;
    e.pos[1] = y;
    memcpy(e.col, col, sizeof(float) * 4);

    entries.push_back(e);
    write(entries.size() - 1, text);
    //printf("Start: %zu, Num vertices: %zu, width: %zu,\n", e.start_dx, e.num_vertices, e.w);//, min_x, max_x);
    update();
    return entries.size()-1;
  }

  int Font::write(unsigned int dx, const std::string& text) {
    FontEntry& e = entries[dx];
    if(e.num_vertices) {
      vertices.erase(vertices.begin()+e.start_dx, vertices.begin()+(e.start_dx+e.num_vertices));
    }

    // Create vertices
    e.w = 0;
    e.start_dx = vertices.size();
    float x = 0;
    float y = 0;
    for(int i = 0; i < text.size(); ++i) {
      char c = text[i];
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata, w, h, c-32, &x, &y, &q, 1);
      CharVertex va(q.x0, q.y1, q.s0, q.t1, e.col);
      CharVertex vb(q.x1, q.y1, q.s1, q.t1, e.col);
      CharVertex vc(q.x1, q.y0, q.s1, q.t0, e.col);
      CharVertex vd(q.x0, q.y0, q.s0, q.t0, e.col);
      CHAR_TRI(va, vb, vc);
      CHAR_TRI(va, vc, vd);

      if(q.x1 > e.w) { 
        e.w = q.x1;
      }
    }
    e.needs_update = true;
    e.num_vertices = vertices.size() - e.start_dx;
    flagChanged();
    return e.num_vertices;
  }

  float Font::getStringWidth(const std::string& str, unsigned int start, unsigned int end) {
    float x = 0.0f;
    float y = 0.0f;
    float txt_w = 0.0f;
    stbtt_aligned_quad q;
    for(unsigned int i = start; i < end; ++i) {
      char c = str[i];
      stbtt_GetBakedQuad(cdata, w, h, c-32, &x, &y, &q, 1);
      txt_w += (q.x1 - q.x0);
    }
    return txt_w;
  }

  void Font::alignRight(unsigned int dx, float rightEdge) {
    FontEntry& e = entries[dx];
    e.align = FEA_RIGHT;
    e.right_edge = rightEdge;
    e.needs_update = true;

    flagChanged();
  }

  void Font::update() {
    if(!needs_update) {
      return ;
    }

    // check if we need to update something for the current font entry.
    for(std::vector<FontEntry>::iterator it = entries.begin(); it != entries.end(); ++it) {
      FontEntry& e = *it;
      if(!e.needs_update) {
        continue;
      }
      if(e.align == FEA_RIGHT) {
        e.pos[0] = e.right_edge - e.w;
      }
      e.needs_update = false;
    }

    // do we need to realloacate our buffer?
    vao.bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    size_t needed = getNumBytes();
    if(needed > allocated_bytes) {
      while(allocated_bytes < needed) {
        allocated_bytes = std::max<size_t>(allocated_bytes * 2, 256 * 10);
      }
      glBufferData(GL_ARRAY_BUFFER, allocated_bytes, NULL, GL_DYNAMIC_DRAW); // discard buffer, prevent syncing delay

      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, pos));
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, tex));
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, col));
      glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CharVertex), (GLvoid*)offsetof(CharVertex, bg));
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, needed, getPtr());
    needs_update = false;
    //    vao.unbind();
  }

  void Font::draw() {
    // Create projection matrix
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    if(vp[2] != win_w || vp[3] != win_h) {
      win_w = vp[2];
      win_h = vp[3];
      createOrtho(win_w, win_h);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
    vao.bind();
    shader.enable();

    shader.uniformMat4fv("u_projection_matrix", pm);

    // set texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex);
    shader.uniform1i("u_font_texture", 3);
  
    // draw
    glPointSize(5.0f);
    size_t dx = 0;
    for(std::vector<FontEntry>::iterator it = entries.begin(); it != entries.end(); ++it) {
      FontEntry& e = *it;
      mm[12] = e.pos[0];
      mm[13] = e.pos[1];
      mm[14] = -0.5f;
      shader.uniformMat4fv("u_model_matrix", mm);
      glDrawArrays(GL_TRIANGLES, dx, vertices.size()); // e.num_vertices);
      dx += e.num_vertices;
    }
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    // vao.unbind();
  }

  bool Font::open(const std::string& filepath, unsigned int size) {
    if(is_initialized) {
      printf("ERROR: you can only use one font file per font object.\n");
      ::exit(0);
    }
    this->size = size;

    // READ TTF DATA
    // -------------
    FILE* fp = fopen(filepath.c_str(), "rb");
    if(!fp) {
      printf("ERROR: cannot open: %s\n", filepath.c_str());
      return false;
    }
    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* font_buffer = new unsigned char[fsize];
    if(!font_buffer) {
      printf("ERROR: cannot allocate font buffer.\n");
      return false;
    }

    fread(font_buffer, 1, fsize, fp);
    fclose(fp);
    fp = 0;

    // CREATE FONT BITMAP
    // -------------------
    unsigned char* pixels = new unsigned char[int(w * h)];
    if(!pixels) {
      printf("ERROR: cannot allocate font pixel buffer.\n");
      return false;
    }

    int char_start = 32;
    int char_end = 96;
    int num_chars = char_end - char_start;
    cdata = new stbtt_bakedchar[num_chars * 2]; // when using num_chars I get a  malloc: *** error for object 0x1008e2808: incorrect checksum for freed object - object was probably modified after being freed.
    printf("Creating a font with size: %d\n", size);
    int r = stbtt_BakeFontBitmap(
                                 font_buffer, 
                                 0, 
                                 size,
                                 pixels, 
                                 w, h, 
                                 char_start, char_end, 
                                 cdata
                                 );
    if(r < 0) {
      printf("ERROR: The baked font doesn't fit in the allocated pixel buffer.\n");
      ::exit(0);
    }
    else if (r == 0) {
      printf("ERROR: No characters fit in the pixel buffer.\n");
      ::exit(0);
    }

    setupGL(pixels, w, h);

    /*
      roxlu::Image img;
      img.copyPixels(pixels, w, h, 1);
      img.save(File::toDataPath("font.png").c_str());
    */

    delete[] pixels;
    delete[] font_buffer;
    pixels = NULL;
    font_buffer = NULL;
    return true;
  }

} // roxlu
