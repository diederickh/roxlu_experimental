#ifdef ROXLU_GL_WRAPPER

#include <pango/TextGLSurface.h>

GLuint TextGLSurface::prog = 0;
GLint TextGLSurface::u_pm = 0;
GLint TextGLSurface::u_mm = 0;
GLint TextGLSurface::u_tex = 0;
GLfloat TextGLSurface::pm[16] = {0};
bool TextGLSurface::pm_set = false;

TextGLSurface::TextGLSurface() 
  :surface_w(0)
  ,surface_h(0)
  ,window_w(0)
  ,window_h(0)
  ,num_bytes(0)
  ,read_dx(0)
  ,write_dx(0)
  ,tex(0)
  ,vao(0)
  ,vbo(0)
  ,is_setup(false)
  ,has_text(false)
  ,needs_update(false)
  ,pbos_filled(false)
{
  if(TextGLSurface::prog == 0) {
    prog = rx_create_shader(TEXT_GL_SURFACE_VS, TEXT_GL_SURFACE_FS);
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");
    glLinkProgram(prog);

    u_mm = glGetUniformLocation(prog, "u_mm");
    u_pm = glGetUniformLocation(prog, "u_pm");
    u_tex = glGetUniformLocation(prog, "u_tex");
  }
}

TextGLSurface::~TextGLSurface() {
  surface_w = 0;
  surface_h = 0;
  window_w = 0;
  window_h = 0;
  num_bytes = 0;
  read_dx = 0;
  write_dx = 0;
  is_setup = false;
  has_text = false;
  needs_update = false;
  pbos_filled = false;

  if(tex) {
    glDeleteTextures(1, &tex);
    tex = 0;
  }

  if(vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
  }

  // @todo cleaning up VAO crashes 
}

void TextGLSurface::setup(const std::string font, unsigned int surfaceW, unsigned int surfaceH, unsigned int windowW, unsigned int windowH) {
  if(!pm_set) {
    rx_ortho(0, windowW, windowH, 0, -1.0, 1.0, pm);    
    pm_set = true;
  }

  text.setup(font, surfaceW, surfaceH);

  surface_w = surfaceW;
  surface_h = surfaceH;
  window_w = windowW;
  window_h = windowH;
  num_bytes = surface_w * surface_h * 4;  // @TODO
  //num_bytes = surface_w * surface_h * 3;  // @TODO

  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  glUseProgram(0);

#if defined(TEXT_GL_SURFACE_USE_PBOS)  
  
  glGenBuffers(TEXT_GL_SURFACE_NUM_PBOS, pbos);
  for(int i = 0; i < TEXT_GL_SURFACE_NUM_PBOS; ++i) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[i]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, num_bytes, NULL, GL_STREAM_DRAW);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, surface_w, surface_h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);

  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  is_setup = true;

  float w = surface_w;
  float h = surface_h;

#if defined(TEXT_GL_SURFACE_USE_NDC_COORDS)  
  float ww = 1.0f;
  float hh = 1.0f;

  GLfloat vertices[] = {
    -ww, -hh, 0.0f, h,
    ww, -hh, w, h, 
    ww, hh, w, 0.0f,
    
    -ww, -hh, 0.0f, h,
    ww, hh, w, 0.0f,
    -ww, hh, 0.0f, 0.0f
  };
#else 
  GLfloat vertices[] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    w, 0.0f, w, 0.0f, 
    w, h, w, h,
    
    0.0f, 0.0f, 0.0f, 0.0f,
    w, h, w, h,
    0.0f, h, 0.0f, h
  };
#endif

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // tex
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);

  
}

void TextGLSurface::draw(int x, int y) {
  if(!is_setup || !has_text) {
    return;
  }

  if(needs_update) {
    text.render();
    setPixels(text.getPixels());

    if(!pbos_filled) {
      setPixels(text.getPixels());
      pbos_filled = true;
    }
    needs_update = false;
  }
  
  glDepthMask(GL_FALSE);
  glDisable(GL_CULL_FACE);
    
  mm.identity();

#if defined(TEXT_GL_SURFACE_USE_NDC_COORDS)  
  float r = (window_h - surface_h - y);
  float xx = -1.0 + (((surface_w * 0.5) + float(x))/window_w) * 2.0f;
  float yy =  -1.0 + (((surface_h * 0.5) +float(r))/window_h) * 2.0f;
  mm.setPosition(xx,yy, 0.0);
  mm.setScale(float(surface_w)/window_w, float(surface_h)/window_h, 1.0);
#else 
  mm.setPosition(x, y, 0.0);
#endif

  glBindVertexArray(vao);
  glUseProgram(prog);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glUniform1i(u_tex, 0);

  glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm.getPtr());
  glUniformMatrix4fv(u_pm, 1, GL_FALSE, pm);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDepthMask(GL_TRUE);
}

void TextGLSurface::setPixels(unsigned char* pixels) {
  if(!pixels) {
    printf("WARNING: TextGLSurface::setPixels(), given pixels is NULL.\n");
    return; 
  }
  if(!tex || surface_w == 0 || surface_h == 0) {
    printf("WARNING: TextGLSurface::setPixels(): cannot set, we're not initialized.\n");
    return;
  }

#if defined(TEXT_GL_SURFACE_USE_PBOS)
  TEXT_GL_SURFACE_TIMER_START
  read_dx = (read_dx + 1) % TEXT_GL_SURFACE_NUM_PBOS;
  write_dx = (read_dx + 1) % TEXT_GL_SURFACE_NUM_PBOS;

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[read_dx]);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,  0); // @todo

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[write_dx]);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, num_bytes, NULL, GL_STREAM_DRAW);

  GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
  if(ptr) {
    memcpy(ptr, pixels, num_bytes);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
  }

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  TEXT_GL_SURFACE_TIMER_END
#else 
  RX_VERBOSE("Not using pbos to render");
  TEXT_GL_SURFACE_TIMER_START
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels);
  TEXT_GL_SURFACE_TIMER_END
#endif
}


#endif // ROXLU_GL_WRAPPER
