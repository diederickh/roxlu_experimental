#include <gpu/GPUDrawer.h>

GPUDrawer::GPUDrawer() 
  :vao(0)
  ,vbo(0)
  ,img(NULL)
  ,viewport_w(0)
  ,viewport_h(0)
  ,flip_x(1)
  ,flip_y(1)
{
  memset(ortho_matrix, 0x00, sizeof(ortho_matrix));
}

GPUDrawer::~GPUDrawer() {
  if(vao) {
    glDeleteVertexArrays(1, &vao);
    vao = 0;
  }
  if(vbo) {
    glDeleteBuffers(1, &vbo);
    vbo = 0;
  }
}

bool GPUDrawer::setup() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  
  gl::Vertices<gl::VertexPT> vertices;
  gl::VertexPT a(Vec3(-0.5, -0.5, 0.0), Vec2(0.0, 0.0));
  gl::VertexPT b(Vec3( 0.5, -0.5, 0.0), Vec2(1.0, 0.0));
  gl::VertexPT c(Vec3( 0.5,  0.5, 0.0), Vec2(1.0, 1.0));
  gl::VertexPT d(Vec3(-0.5,  0.5, 0.0), Vec2(0.0, 1.0));
  vertices.push_back(a);
  vertices.push_back(b);
  vertices.push_back(c);
  vertices.push_back(a);
  vertices.push_back(c);
  vertices.push_back(d);

  glBufferData(GL_ARRAY_BUFFER, vertices.getNumBytes(), vertices.getPtr(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // tex
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gl::VertexPT), (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(gl::VertexPT), (GLvoid*)12);

  return true;
}

bool GPUDrawer::draw(GPUImage& img, GLuint* ids) {

  if(!viewport_w) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    viewport_w = viewport[2];
    viewport_h = viewport[3];
    rx_ortho(0, viewport_w, viewport_h, 0, -1.0, 1.0, ortho_matrix);  
  }

  model_matrix.identity();
  model_matrix.setScale(scale.x * flip_x, scale.y * flip_y, scale.z);
  model_matrix.setPosition(position.x, position.y, position.z);
  model_matrix.rotateZ(rotation.z);
  
  img.useProgram();
  img.setProjectionMatrix(ortho_matrix); 
  img.setModelMatrix(model_matrix.getPtr());
  img.bindTextures(ids);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  return true;
}
