#include <gpu/GPUImage_RGB24.h>

GPUImage_RGB24::GPUImage_RGB24()
  :GPUImage_Base(AV_PIX_FMT_RGB24)
  ,u_mm(-1)
  ,u_pm(-1)
  ,u_tex(-1)
{
}

GPUImage_RGB24::~GPUImage_RGB24() {
  shutdown();
}

bool GPUImage_RGB24::setup() {
  prog = rx_create_shader(RGB_VS, RGB_FS, vert_id, frag_id);
  glBindAttribLocation(prog, 0, "a_pos");
  glBindAttribLocation(prog, 1, "a_tex");
  glLinkProgram(prog);
  glUseProgram(prog);

  u_tex = glGetUniformLocation(prog, "u_tex");
  if(u_tex < 0) {
    RX_ERROR("Error while trying to get the u_tex uniform; not used?");
    return false;
  }

  u_pm = glGetUniformLocation(prog, "u_pm");
  if(u_pm < 0) {
    RX_ERROR("Error while trying to get the u_pm uniform; not used?");
    return false;
  }

  u_mm = glGetUniformLocation(prog, "u_mm");
  if(u_mm < 0) {
    RX_ERROR("Error while trying to get the u_mm uniform; not used?");
    return false;
  }

  glUniform1i(u_tex, 0);

  return true;
}

void GPUImage_RGB24::genTextures(GLuint* id, int w, int h) {
  glGenTextures(1, id);
  glBindTexture(GL_TEXTURE_2D, *id);
  glTexImage2D(GL_TEXTURE_2D, 0,
               getInternalFormat(), w, h, 0, 
               getFormat(), getType(), NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GPUImage_RGB24::setPixels(GLuint* id, char* pixels, int w, int h) {
  glBindTexture(GL_TEXTURE_2D, *id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, getFormat(), getType(), pixels);
}

void GPUImage_RGB24::bindTextures(GLuint* id) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *id);
}

void GPUImage_RGB24::setProjectionMatrix(const float* pm) {
  glUniformMatrix4fv(u_pm, 1, GL_FALSE, pm);
}

void GPUImage_RGB24::setModelMatrix(const float* mm) {
  glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm);
}

void GPUImage_RGB24::setTextureUniform(int unit) {
  glUniform1i(u_tex, unit);
}

