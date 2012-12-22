#ifndef ROXLU_RDEBUG_DRAWER_H
#define ROXLU_RDEBUGDRAWER_H

#include "RParticles.h"
#include <roxlu/Roxlu.h>
#include <string>

const std::string DD_VS = ""
  "uniform mat4 u_pm;"
  "uniform mat4 u_vm;"
  "uniform mat3 u_nm;"
  "uniform mat4 u_mm;"
  "attribute vec4 a_pos;"
  "attribute vec3 a_norm;"
  "varying vec3 v_norm;"
  "varying vec3 v_r;"
  "varying vec3 v_s;"
  "varying vec3 v_v;"
  "varying vec3 v_eye_pos;"
  "varying vec3 v_light_pos;"
  ""
  "void main() {"
  "   gl_Position = u_pm * u_vm * u_mm * a_pos;"
  "   v_norm = u_nm * a_norm;"
  "   mat4 mv = u_vm * u_mm; "
  "   vec4 light_pos = mv * vec4(0.0, 0.0, 300.0, 1.0);"
  "   vec4 eye_pos = mv * a_pos; "
  "   v_eye_pos = eye_pos.xyz;"
  "   v_light_pos = light_pos.xyz;"
  "   v_s = normalize((light_pos.xyz - eye_pos.xyz)); "
  "   v_v = -normalize(eye_pos.xyz); "
  "   v_r = reflect(-v_s, v_norm); "
  "}"
  "";

const std::string DD_FS = ""
  "varying vec3 v_norm;"
  "uniform int u_mode; "
  "varying vec3 v_r;"
  "varying vec3 v_s;"
  "varying vec3 v_v;"
  "varying vec3 v_eye_pos;"
  "varying vec3 v_light_pos;"
  "void main() {"
  "   vec3 s = normalize((v_light_pos.xyz - v_eye_pos.xyz)); "
  "   vec3 v = -normalize(v_eye_pos.xyz); "
  "   vec3 r = reflect(-s, v_norm); "
  "   float sdotn = max(dot(s, v_norm), 0.1); "
  "   vec3 spec = vec3(0.0);"
  "   if(sdotn > 0.0) { "
  "      spec = vec3(1.0, 1.0, 1.0) * pow(max(dot(r,v), 0.0), 1.8); "
  "   }"
  "  gl_FragColor.a = 1.0;"
  "  gl_FragColor.rgb = max(sdotn,0.1) * (0.5 + 0.5 * v_norm); "
  "  if(u_mode == 1) { "
  "      gl_FragColor.rgb = max(sdotn,0.1) * vec3(1.0, 0.3, 0.2);"
  "  }"
  "  gl_FragColor.rgb += spec; "
  "}"
  "";

class RDebugDrawer {
 public:
  RDebugDrawer();
  ~RDebugDrawer();
  void setup();
  void draw(const float* pm, const float* vm, const float* nm, std::vector<RParticle>& particles);
 public:
  int mode;
 private:
  void setupShader();
  void setupBuffers();
 private:
  GLuint prog;
  GLuint vao;
  GLuint vbo;
  GLint pos_id;
  GLint norm_id;
  GLint vm_id;
  GLint pm_id;
  GLint mm_id;
  GLint nm_id;
  GLint mode_id;
  size_t num_vertices;

};

#endif
