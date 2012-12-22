#include "RDebugDrawer.h"

RDebugDrawer::RDebugDrawer() 
  :num_vertices(0)
  ,mode(0)
{
}

RDebugDrawer::~RDebugDrawer() {
}

void RDebugDrawer::draw(const float* pm, const float* vm, const float* nm, std::vector<RParticle>& particles) {
  glBindVertexArray(vao);
  glUseProgram(prog);
  glUniformMatrix4fv(vm_id, 1, GL_FALSE, vm);
  glUniformMatrix4fv(pm_id, 1, GL_FALSE, pm);
  glUniformMatrix3fv(nm_id, 1, GL_FALSE, nm);
  glUniform1i(mode_id, mode);
  Mat4 model;
  Vec3 up(0.0, 1.0, 0.0);
  Vec3 right(1.0, 0.0, 0.0);
  for(std::vector<RParticle>::iterator it = particles.begin(); it != particles.end(); ++it) {
    RParticle& p = *it;

    right = cross(p.vel_norm, up).normalize();
    up = cross(right, p.vel_norm);
    model.setAxis(right, up, p.vel_norm);
    model.setPosition(p.pos);

    glUniformMatrix4fv(mm_id, 1, GL_FALSE, model.getPtr());
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
  }
}

void RDebugDrawer::setup() {
  glEnable(GL_DEPTH_TEST);
  setupShader();
  setupBuffers();
}

void RDebugDrawer::setupShader() {
  GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  const char* vss = DD_VS.c_str();
  const char* fss = DD_FS.c_str();
  glShaderSource(vert_id, 1, &vss, NULL);
  glShaderSource(frag_id, 1, &fss, NULL);
  glCompileShader(vert_id); eglGetShaderInfoLog(vert_id);
  glCompileShader(frag_id); eglGetShaderInfoLog(frag_id);
  prog = glCreateProgram();
  glAttachShader(prog, vert_id);
  glAttachShader(prog, frag_id);
  glLinkProgram(prog);

  glUseProgram(prog);
  pos_id = glGetAttribLocation(prog, "a_pos");
  norm_id = glGetAttribLocation(prog, "a_norm");
  pm_id = glGetUniformLocation(prog, "u_pm");
  vm_id = glGetUniformLocation(prog, "u_vm");
  mm_id = glGetUniformLocation(prog, "u_mm");
  nm_id = glGetUniformLocation(prog, "u_nm");
  mode_id = glGetUniformLocation(prog, "u_mode");

}

void RDebugDrawer::setupBuffers() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  float* verts;
  num_vertices = rx_load_obj_file("sphere.obj", &verts, true);
  if(num_vertices) {
    glBufferData(GL_ARRAY_BUFFER, num_vertices * (sizeof(float) * 6), verts, GL_STATIC_DRAW);
  }

  glEnableVertexAttribArray(pos_id);
  glEnableVertexAttribArray(norm_id);
  glVertexAttribPointer(pos_id, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)0);
  glVertexAttribPointer(norm_id, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (GLvoid*)12);
}
