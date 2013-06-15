#include <videocapture/VideoCaptureGLSurface.h>

// @todo instead of check if setup() is called, add the necessary things to the c'tor.

GLuint VideoCaptureGLSurface::prog = 0;
GLint VideoCaptureGLSurface::u_pm = 0;
GLint VideoCaptureGLSurface::u_mm = 0;
GLint VideoCaptureGLSurface::u_rot_matrix = 0;
GLint VideoCaptureGLSurface::u_scale_matrix = 0;
GLint VideoCaptureGLSurface::u_tex = 0;
GLfloat VideoCaptureGLSurface::pm[16] = {0};

VideoCaptureGLSurface::VideoCaptureGLSurface() 
  :surface_w(0)
  ,surface_h(0)
  ,window_w(0)
  ,window_h(0)
  ,vbo(0)
  ,vao(0)
  ,tex(0)
  ,write_dx(0)
  ,read_dx(0)
  ,is_setup(false)
{
  memset(mm, 0, sizeof(float) * 16);
  memset(rot_matrix, 0, sizeof(float) * 16);
  memset(scale_matrix, 0.0, sizeof(float) * 16);

  mm[0]  = rot_matrix[0]  = scale_matrix[0]  = 1.0f; 
  mm[5]  = rot_matrix[5]  = scale_matrix[5]  = 1.0f; 
  mm[10] = rot_matrix[10] = scale_matrix[10] = 1.0f;
  mm[15] = rot_matrix[15] = scale_matrix[15] = 1.0f;
}

VideoCaptureGLSurface::~VideoCaptureGLSurface() {
  reset();
}


//void VideoCaptureGLSurface::setup(int w, int h, VideoCaptureFormat format) {
void VideoCaptureGLSurface::setup(int w, int h, GLenum internalFormat, GLenum format, GLenum type) { 
  //  fmt = format;
  surface_w = w;
  surface_h = h;
  num_bytes = w * h * 4; // when using yuv, we allocate a bit too much
  tex_internal_format = internalFormat;
  tex_format = format;
  tex_type = type;

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
    u_rot_matrix = glGetUniformLocation(prog, "u_rot_matrix");
    u_scale_matrix = glGetUniformLocation(prog, "u_scale_matrix");
    u_tex = glGetUniformLocation(prog, "u_tex");
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);

  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, internalFormat, surface_w, surface_h, 0, format, type, NULL);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glUseProgram(prog);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glUniform1i(u_tex, 0);

  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  is_setup = true;
  setupGL();
}

// @todo we could optimize by not updating the model matrix when it didn't change
void VideoCaptureGLSurface::draw(int x, int y, int w, int h) {
  if(!is_setup) {
    RX_ERROR("cannot draw setup() not called");
    return;
  }

  glBindVertexArray(vao);
  glUseProgram(prog);

  if(w == 0 && h == 0) {
    w = surface_w;
    h = surface_h;
  }
  mm[12] = x + w * 0.5;
  mm[13] = y + h * 0.5;
 
  scale_matrix[0] = w; 
  scale_matrix[5] = h; 

  glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm);
  glUniformMatrix4fv(u_scale_matrix, 1, GL_FALSE, scale_matrix);
  glUniformMatrix4fv(u_rot_matrix, 1, GL_FALSE, rot_matrix);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}


void VideoCaptureGLSurface::setupGL() {
  
  if(vao != 0) {
    RX_ERROR("cannot re-setup GL.. already initialized.\n");
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

  float w = 0.5f; // surface_w;
  float h = 0.5f; // surface_h;
  vertices[0].set(-w, -h, 0.0f, 0.0f);
  vertices[1].set( w, -h, surface_w, 0.0f);
  vertices[2].set( w,  h, surface_w, surface_h);
  vertices[3].set(-w, -h, 0.0f, 0.0f);
  vertices[4].set( w,  h, surface_w, surface_h);
  vertices[5].set(-w,  h, 0.0f, surface_h);
  
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0); 
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);
}

void VideoCaptureGLSurface::setPixels(unsigned char* pixels, size_t nbytes) {
  if(!is_setup) {
    RX_ERROR("cannot draw setup() not called");
    return;
  }

  if(vao == 0) {
    setupGL();
  }

  read_dx = (read_dx + 1) % VIDEO_CAP_NUM_PBOS;
  write_dx = (read_dx + 1) % VIDEO_CAP_NUM_PBOS;

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[read_dx]);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);

  /*
#if defined(__APPLE__)
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_APPLE, 0);
#elif defined(_WIN32)
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, GL_RGB, GL_UNSIGNED_BYTE, 0);
  #endif
  */

  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, surface_w, surface_h, tex_format, tex_type, 0);
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
  if(is_setup) {
    RX_ERROR("cannot reset, first call setup()");
    return;
  }

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
  //  fmt = VC_NONE;
  write_dx = 0;
  read_dx = 0;
}

void VideoCaptureGLSurface::flip(bool vert, bool hor) {
  if(vbo == 0) {
    RX_ERROR("cannot flip the surface, first call setup()");
    return;
  }

  if(vert) { 
    vertices[0].t = surface_h;
    vertices[1].t = surface_h;
    vertices[2].t = 0.0f;
    vertices[3].t = surface_h;
    vertices[4].t = 0.0f;
    vertices[5].t = 0.0f;
  }
  else {
    vertices[0].t = 0.0f;
    vertices[1].t = 0.0f;
    vertices[2].t = surface_h;
    vertices[3].t = 0.0f;
    vertices[4].t = surface_h;
    vertices[5].t = surface_h;
  }

  if(hor) {
    vertices[0].s = surface_w;
    vertices[1].s = 0.0f;
    vertices[2].s = 0.0f;
    vertices[3].s = surface_w;
    vertices[4].s = 0.0f;
    vertices[5].s = surface_w;
  }
  else {
    vertices[0].s = 0.0f;
    vertices[1].s = surface_w;
    vertices[2].s = surface_w;
    vertices[3].s = 0.0f;
    vertices[4].s = surface_w;
    vertices[5].s = 0.0f;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); //, GL_DYNAMIC_DRAW);

}

void VideoCaptureGLSurface::rotate(float degrees) {
  float angle = (VIDEO_CAP_PI / 180.0f) * degrees;
  float ca = cos(angle);
  float sa = sin(angle);
  rot_matrix[0] = ca;
  rot_matrix[1] = sa;
  rot_matrix[4] = -sa;
  rot_matrix[5] = ca;
}
