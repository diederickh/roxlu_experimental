#include <glr/Drawer.h>
#include <glr/Texture.h>
#include <glr/Mesh.h>
#include <glr/Font.h>

namespace gl {
  
  Drawer::Drawer() 
    :window_w(0)
    ,window_h(0)
    ,texture_mesh(NULL)
    ,immediate_must_fill(true)
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
    shader_p.create(GL_VS_P, GL_FS_P); 
    shader_p.bindAttribLocation("a_pos", 0);
    shader_p.link();

    shader_pt.create(GL_VS_PT, GL_FS_PT);
    shader_pt.bindAttribLocation("a_pos", 0);
    shader_pt.bindAttribLocation("a_tex", 1);
    shader_pt.link();

    shader_tex.create(GL_VS_PT, GL_FS_PT); 
    shader_tex.bindAttribLocation("a_pos", 0);
    shader_tex.bindAttribLocation("a_tex", 1);
    shader_tex.link();

    shader_immediate.create(GL_VS_CP, GL_FS_CP); 
    shader_immediate.bindAttribLocation("a_col", 0);
    shader_immediate.bindAttribLocation("a_pos", 1);
    shader_immediate.link();
    
    setupShader(shader_p);
    setupShader(shader_pt);
    setupShader(shader_tex);
    setupShader(shader_immediate);

    // MESHES
    float tw = 1.0f;
    float th = 1.0f;
    texture_mesh = new Mesh<VertexPT>();
    texture_mesh->setup(GL_STATIC_DRAW);
    texture_mesh->push_back(VertexPT(Vec3(0.0f,   th),  Vec2(0.0f, 1.0f)) );
    texture_mesh->push_back(VertexPT(Vec3(tw,     th),  Vec2(1.0f, 1.0f)) );
    texture_mesh->push_back(VertexPT(Vec3(tw,   0.0f),  Vec2(1.0f, 0.0f)) );
    texture_mesh->push_back(VertexPT(Vec3(0.0f,   th),  Vec2(0.0f, 1.0f)) );
    texture_mesh->push_back(VertexPT(Vec3(tw,   0.0f),  Vec2(1.0f, 0.0f)) );
    texture_mesh->push_back(VertexPT(Vec3(0.0f, 0.0f),  Vec2(0.0f, 0.0f)) );
    texture_mesh->update();

    immediate_mesh = new Mesh<VertexCP>();
    immediate_mesh->setup(GL_STREAM_DRAW); 

    // SHAPES
    int circle_resolution = 64;
    float rad = TWO_PI / circle_resolution;
    for(int i = 0; i < circle_resolution; ++i) {
      VertexCP v(Vec3(1.0f, 1.0f, 1.0f), Vec3(cos(i * rad), sin(i * rad), 0.0f));
      immediate_circle.push_back(v);
    }

  }

  Drawer::~Drawer() {
    delete texture_mesh;
    delete immediate_mesh;
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
    texture_mesh->model_matrix[12] = x;
    texture_mesh->model_matrix[13] = y;
    texture_mesh->model_matrix[0] = ww;
    texture_mesh->model_matrix[5] = hh;

    shader_tex.use();
    shader_tex.setModelMatrix(texture_mesh->model_matrix.getPtr());
    shader_tex.activateTexture(tex, GL_TEXTURE0);
    
    texture_mesh->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    texture_mesh->unbind();

    shader_tex.unuse();
  }

  void Drawer::drawArrays(Mesh<VertexP>& mesh, GLenum mode, GLint first, GLsizei count) {
    shader_p.use();
    mesh.bind();
    mesh.update();
    glDrawArrays(mode, first, count);
    mesh.unbind();
    shader_p.unuse();
  }

  void Drawer::drawArrays(Mesh<VertexPT>& mesh, GLenum mode, GLint first, GLsizei count) {
    shader_pt.use();
    mesh.bind();
    mesh.update();
    glDrawArrays(mode, first, count);
    mesh.unbind();
    shader_pt.unuse();
  }


  void Drawer::createOrthographicMatrix() {
    ortho_matrix.orthoTopLeft(window_w, window_h, 0.0f, 100.0f);
  }

  void Drawer::onResize(unsigned int w, unsigned int h) {
    window_w = w;
    window_h = h;
    createOrthographicMatrix();
  }


  // IMMEDIATE MODE (slower then using custom meshes)
  // ---------------------------------------------------
  void Drawer::begin(GLenum mode) {
    immediate_entry.mode = mode;
    immediate_entry.start_dx = immediate_mesh->size();
    immediate_entry.num_elements = 0;
  }

  void Drawer::color(Vec4 color) {
    immediate_color = color;
  }

  void Drawer::vertex(Vec3 position) {
    VertexCP vertex_cp(immediate_color, position);
    immediate_entry.num_elements = immediate_mesh->size() + 1;
    immediate_mesh->push_back(vertex_cp);
  }

  void Drawer::end() {
    immediate_entries.push_back(immediate_entry);
    draw();
  }

  void Drawer::draw() {
    shader_immediate.use();
    immediate_mesh->bind();
    immediate_mesh->update();

    size_t offset = 0;
    std::vector<DrawEntry>::iterator it = immediate_entries.begin();
    while(it != immediate_entries.end()) {
      DrawEntry& e = *it;
      glDrawArrays(e.mode, offset, e.num_elements);
      offset += e.num_elements;
      ++it;
    }

    immediate_mesh->unbind();
    shader_immediate.unuse();
    
    immediate_mesh->clear();
    immediate_entries.clear();
  }

  void Drawer::fill() {
    immediate_must_fill = true;
  }

  void Drawer::nofill() {
    immediate_must_fill = false;
  }

  // SHAPES (immediate mode)
  // ---------------------------------------------------
  void Drawer::drawCircle(float x, float y, float radius) {
    begin((immediate_must_fill) ? GL_POLYGON : GL_LINE_LOOP);
    for(size_t i = 0; i < immediate_circle.size(); ++i) {
      VertexCP v = immediate_circle[i];
      Vec3 pos(x + v.pos.x * radius, y + v.pos.y * radius, 0.0f);
      vertex(pos);
    }
    end();
  }

  void Drawer::drawRectangle(float x, float y, float w, float h) {
    begin((immediate_must_fill) ? GL_POLYGON : GL_LINE_LOOP);
    vertex(Vec3(x, y, 0));         // top left
    vertex(Vec3(x + w, y, 0));;    // top right 
    vertex(Vec3(x + w, y + h, 0)); // bottom right
    vertex(Vec3(x, y + h, 0));     // bottom left
    end();
  }

  // ---------------------------------------------------

  void glr_init() {
    glr_context = new Drawer();
    glr_font.init();
  }

  void glr_draw_circle(float x, float y, float radius) {
    assert(glr_context);
    glr_context->drawCircle(x, y, radius);
  }

  void glr_draw_rectangle(float x, float y, float w, float h) {
    assert(glr_context);
    glr_context->drawRectangle(x, y, w, h);
  }

  void glr_fill() {
    assert(glr_context);
    glr_context->fill();
  }

  void glr_nofill() {
    assert(glr_context);
    glr_context->nofill();
  }

  void glr_begin(GLenum mode) {
    assert(glr_context);
    glr_context->begin(mode);
  }

  void glr_color(Vec4 color) {
    assert(glr_context);
    glr_context->color(color);
  }

  void glr_vertex(Vec3 position) {
    assert(glr_context);
    glr_context->vertex(position);
  }

  void glr_end() {
    assert(glr_context);
    glr_context->end();
  }

  Drawer* glr_context = NULL;

} // gl
