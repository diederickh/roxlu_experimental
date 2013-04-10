#include <glr/Drawer.h>

namespace gl {
  
  Drawer::Drawer() 
    :window_w(0)
    ,window_h(0)
  {


    // ORTHO MATRIX
    if(window_w == 0 || window_h == 0) {
      GLint vp[4];
      glGetIntegerv(GL_VIEWPORT, vp);

      window_w = vp[2];
      window_h = vp[3];
    
      onResize(window_w, window_h); 
    }

    // SHADERS
    shader_p.create(GL_VS_P, GL_FS_P, VERTEX_P);
    shader_pt.create(GL_VS_PT, GL_FS_PT, VERTEX_PT);

    setupShader(shader_p);
    setupShader(shader_pt);

    // MESHES
    float tw = 1.0f;
    float th = 1.0f;
    texture_mesh.setup(GL_STATIC_DRAW, VERTEX_PT);
    texture_mesh.push_back(VertexPT(Vec3(0.0f,   th),  Vec2(0.0f, 1.0f)) );
    texture_mesh.push_back(VertexPT(Vec3(tw,     th),  Vec2(1.0f, 1.0f)) );
    texture_mesh.push_back(VertexPT(Vec3(tw,   0.0f),  Vec2(1.0f, 0.0f)) );
    texture_mesh.push_back(VertexPT(Vec3(0.0f,   th),  Vec2(0.0f, 1.0f)) );
    texture_mesh.push_back(VertexPT(Vec3(tw,   0.0f),  Vec2(1.0f, 0.0f)) );
    texture_mesh.push_back(VertexPT(Vec3(0.0f, 0.0f),  Vec2(0.0f, 0.0f)) );
  }

  Drawer::~Drawer() {
  }

  void Drawer::setupShader(Shader& shader) {
    Mat4 identity;
    shader.setProjectionMatrix(ortho_matrix.getPtr());
    shader.setViewMatrix(identity.getPtr());
    shader.setModelMatrix(identity.getPtr());
  }

  void Drawer::drawTexture(Texture& tex, float x, float y, float w, float h) {
    float ww = (w == 0) ? tex.getWidth() : w;
    float hh = (h == 0) ? tex.getHeight() : h;
    texture_mesh.model_matrix[12] = x;
    texture_mesh.model_matrix[13] = y;
    texture_mesh.model_matrix[0] = ww;
    texture_mesh.model_matrix[5] = hh;

    shader_pt.use();
    shader_pt.setModelMatrix(texture_mesh.model_matrix.getPtr());
    shader_pt.activateTexture(tex, GL_TEXTURE0);
    
    texture_mesh.bind();
    texture_mesh.drawArrays(GL_TRIANGLES, 0, 6);
    texture_mesh.unbind();

    shader_pt.unuse();
  }

  void Drawer::drawArrays(Mesh<VertexP>& mesh, GLenum mode, GLint first, GLsizei count) {
    shader_p.use();
    mesh.bind();
    mesh.drawArrays(mode, first, count);
    mesh.unbind();
    shader_p.unuse();
  }

  void Drawer::createOrthographicMatrix() {
    ortho_matrix.orthoTopLeft(window_w, window_h, 0.0f, 100.0f);
  }

  void Drawer::onResize(unsigned int w, unsigned int h) {
    window_w = w;
    window_h = h;
    createOrthographicMatrix();
  }

} // gl
