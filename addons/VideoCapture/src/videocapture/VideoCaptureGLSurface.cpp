#include <videocapture/VideoCaptureGLSurface.h>

GLuint VideoCaptureGLSurface::prog = 0;
GLint VideoCaptureGLSurface::u_pm = 0;
GLint VideoCaptureGLSurface::u_mm = 0;
GLint VideoCaptureGLSurface::u_tex = 0;
GLfloat VideoCaptureGLSurface::pm[16] = {0};

VideoCaptureGLSurface::VideoCaptureGLSurface() 
  :surface_w(0)
  ,surface_h(0)
  ,window_w(0)
  ,window_h(0)
  ,fmt(VC_NONE)
  ,vbo(0)
  ,vao(0)
  ,tex(0)
  ,write_dx(0)
  ,read_dx(0)
{
  memset(mm, 0, sizeof(float) * 16);
  mm[0] = 1.0f; 
  mm[5] = 1.0f; 
  mm[10] = 1.0f;
  mm[15] = 1.0f;
}

VideoCaptureGLSurface::~VideoCaptureGLSurface() {
  reset();
}


void VideoCaptureGLSurface::setup(int w, int h, VideoCaptureFormat format) {
  fmt = format;
  surface_w = w;
  surface_h = h;
  num_bytes = w * h * 4; // when using yuv, we allocate a bit too much

  if(prog == 0) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs, 1, &VIDEO_CAP_VS, NULL);
    glShaderSource(fs, 1, &VIDEO_CAP_FS, NULL);
    glCompileShader(vs); eglGetShaderInfoLog(vs);
    glCompileShader(fs); eglGetShaderInfoLog(fs);
    prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");
    glLinkProgram(prog);
    
    u_pm = glGetUniformLocation(prog, "u_pm");
    u_mm = glGetUniformLocation(prog, "u_mm");
    u_tex = glGetUniformLocation(prog, "u_tex");
  }

  glGenTextures(1, &tex);

  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
#if defined(__APPLE__)
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, surface_w, surface_h, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, NULL);
#elif defined(_WIN32)
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, surface_w, surface_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#endif
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glUseProgram(prog);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glUniform1i(u_tex, 0);

  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
}

// @todo we could optimize by not updating the model matrix when it didn't change
void VideoCaptureGLSurface::draw(int x, int y, int w, int h) {
  if(vao == 0) {
    setupGL();
  }

  glBindVertexArray(vao);
  glUseProgram(prog);

  if(w == 0 && h == 0) {
    w = surface_w;
    h = surface_h;
  }
  mm[0] = w; 
  mm[5] = h; 
  mm[10] = 1.0f;
  mm[15] = 1.0f;    
  mm[12] = x;
  mm[13] = y;

  glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void VideoCaptureGLSurface::setupGL() {
  
  if(vao != 0) {
    RX_ERROR(("cannot re-setup GL.. already initialized.\n"));
    return;
  }
  
  if(pm[15] == 0.0f) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    float l = 0;
    float r = viewport[2];
    float b = viewport[3];
    float n = -1.0;
    float f = 1.0;
    float t = 0.0;

    pm[0] = (2.0f / (r - l));
    pm[5] = (2.0f / (t - b));
    pm[10] = (-2.0f / (f - n));
    pm[12] = - ((r + l) / (r - l));
    pm[13] = - ((t + b) / (t - b));
    pm[14] = - ((f + n) / (f - n));
    pm[15] = 1.0f;

    glUseProgram(prog);
    glUniformMatrix4fv(u_pm, 1, GL_FALSE, pm);
  }
  
  glGenBuffers(VIDEO_CAP_NUM_PBOS, pbos);
  for(int i = 0; i < VIDEO_CAP_NUM_PBOS; ++i) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[i]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, num_bytes, NULL, GL_STREAM_DRAW);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  float w = 1.0f; // surface_w;
  float h = 1.0f; // surface_h;
  GLfloat vertices[] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    w, 0.0f, surface_w, 0.0f, 
    w, h, surface_w, surface_h,
    
    0.0f, 0.0f, 0.0f, 0.0f,
    w, h, surface_w, surface_h,
    0.0f, h, 0.0f, surface_h
  };
  
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0); 
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);
}

void VideoCaptureGLSurface::setPixels(unsigned char* pixels, size_t nbytes) {
  if(vao == 0) {
    setupGL();
  }

  read_dx = (read_dx + 1) % VIDEO_CAP_NUM_PBOS;
  write_dx = (read_dx + 1) % VIDEO_CAP_NUM_PBOS;

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[read_dx]);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
#if defined(__APPLE__)
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, 0);
#elif defined(_WIN32)
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, GL_RGB, GL_UNSIGNED_BYTE, 0);
#endif

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[write_dx]);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, nbytes, NULL, GL_STREAM_DRAW); 
  GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
  if(ptr) {
    memcpy(ptr, pixels, nbytes);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


void VideoCaptureGLSurface::reset() {
  RX_VERBOSE(("reset"));
  if(tex) {
    glDeleteTextures(1, &tex);
    tex = 0;
  }

  if(vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
  }
  
  surface_w = 0;
  surface_h = 0;
  window_w = 0;
  window_h = 0;

  vbo = 0;
  vao = 0;
  fmt = VC_NONE;
  write_dx = 0;
  read_dx = 0;
}

