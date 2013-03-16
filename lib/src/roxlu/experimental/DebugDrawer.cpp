#include <roxlu/experimental/DebugDrawer.h>

#ifdef ROXLU_WITH_OPENGL

DebugDrawer::DebugDrawer()
  :is_initialized(false)
  ,must_fill(true)
  ,bytes_allocated(0)
  ,pos_id(0)
  ,col_id(0)
  ,projection_matrix_id(0)
  ,view_matrix_id(0)
  ,vbo(0)
  ,vao(0)
  ,vert_id(0)
  ,frag_id(0)
  ,prog_id(0)
  ,win_w(0)
  ,win_h(0)
  ,shader_tex(0)
  ,vbo_tex(0)
  ,vao_tex(0)
  ,prog_tex(0)
  ,pm_id_tex(0)
  ,mm_id_tex(0)
  ,tex_uniform(0)
{
  memset(ortho_matrix, 0, sizeof(float) * 16);
  memset(tex_matrix, 0, sizeof(float) * 16);
  memset(view_matrix, 0, sizeof(float) * 16);
  tex_matrix[0] = tex_matrix[5] = tex_matrix[10] = tex_matrix[15] = 1.0f;
  view_matrix[0] = view_matrix[5] = view_matrix[10] = view_matrix[15] = 1.0f;

  int circle_resolution = 64;
  float rad = TWO_PI / circle_resolution;

  for(int i = 0; i < circle_resolution; ++i) {
    VertexPC v;
    v.setPos(cos(i * rad), sin(i * rad), 0.0f);
    v.setCol(1.0f, 1.0f, 1.0f, 1.0f);
    circle.push_back(v);
  }
}

DebugDrawer::~DebugDrawer() {
}

void DebugDrawer::setupOpenGL() {
  // ---------------------------------------------------------
  // GENERIC DRAWING OPENGL SETUP 
  // ---------------------------------------------------------

  // Create simple shader.
  vert_id = glCreateShader(GL_VERTEX_SHADER);
  frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  
  const char* vss = DEBUG_VS.c_str();
  const char* fss = DEBUG_FS.c_str();
  int vlen = DEBUG_VS.size();
  int flen = DEBUG_FS.size();

  glShaderSource(vert_id, 1, &vss, NULL);
  glShaderSource(frag_id, 1, &fss, NULL);

  glCompileShader(vert_id);
  eglGetShaderInfoLog(vert_id);

  glCompileShader(frag_id);
  eglGetShaderInfoLog(frag_id);

  prog_id = glCreateProgram();

  glAttachShader(prog_id, vert_id);
  glAttachShader(prog_id, frag_id);

  glLinkProgram(prog_id);
  projection_matrix_id =  glGetUniformLocation(prog_id, "u_projection_matrix");
  view_matrix_id = glGetUniformLocation(prog_id, "u_view_matrix");
    
  // Buffers
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // set attributes
  pos_id = glGetAttribLocation(prog_id, "a_pos");
  col_id = glGetAttribLocation(prog_id, "a_col");
  glEnableVertexAttribArray(pos_id);
  glEnableVertexAttribArray(col_id);
  glVertexAttribPointer(pos_id, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (GLvoid*)offsetof(VertexPC, pos));
  glVertexAttribPointer(col_id, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (GLvoid*)offsetof(VertexPC, col));

  // ---------------------------------------------------------
  // TEXTURE DRAWING OPENGL SETUP
  // ---------------------------------------------------------
  is_initialized = true;
  GLuint tex_vs_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint tex_fs_id = glCreateShader(GL_FRAGMENT_SHADER);
  vss = DEBUG_TEX_VS.c_str();
  fss = DEBUG_TEX_FS.c_str();
  glShaderSource(tex_vs_id, 1, &vss, NULL);
  glShaderSource(tex_fs_id, 1, &fss, NULL);
  glCompileShader(tex_vs_id); eglGetShaderInfoLog(tex_vs_id);
  glCompileShader(tex_fs_id); eglGetShaderInfoLog(tex_fs_id);

  prog_tex = glCreateProgram();
  glAttachShader(prog_tex, tex_vs_id);
  glAttachShader(prog_tex, tex_fs_id);
  glBindAttribLocation(prog_tex, 0, "a_pos");
  glBindAttribLocation(prog_tex, 1, "a_tex");
  glLinkProgram(prog_tex);

  pm_id_tex = glGetUniformLocation(prog_tex, "u_projection_matrix");
  mm_id_tex= glGetUniformLocation(prog_tex, "u_model_matrix");
  tex_uniform = glGetUniformLocation(prog_tex, "u_tex");
  
  glGenVertexArrays(1, &vao_tex);
  glBindVertexArray(vao_tex);

  float tw = 1.0f;
  float th = 1.0f;
  float tex_verts[] = {

    0.0f, th,   0.0f, 1.0f,
    tw,   th,   1.0f, 1.0f, 
    tw,   0.0f, 1.0f, 0.0f, 

    0.0f, th,   0.0f, 1.0f,
    tw,   0.0f, 1.0f, 0.0f, 
    0.0f, 0.0f, 0.0f, 0.0f, 
  };

  glGenBuffers(1, &vbo_tex);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_verts), tex_verts, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);



  checkSize();

  is_initialized = true;
}

void DebugDrawer::createOrtho(int winWidth, int winHeight) {
  float n = -1.0;
  float f = 1.0;
  float ww = winWidth;
  float hh = winHeight;
  float fmn = f - n;
  ortho_matrix[15] = 1.0f;
  ortho_matrix[0]  = 2.0f / ww;
  ortho_matrix[5]  = 2.0f / -hh;
  ortho_matrix[10] = -2.0f / fmn;
  ortho_matrix[12] = -(ww)/ww;
  ortho_matrix[13] = -(hh)/-hh;
  ortho_matrix[14] = -(f+n)/fmn;
}

void DebugDrawer::checkSize() {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  if(vp[2] != win_w || vp[3] != win_h) {
    win_w = vp[2];
    win_h = vp[3];
    createOrtho(win_w, win_h);
  }

}

void DebugDrawer::begin(GLenum type) {
  entry.type = type;
  entry.start_dx = vertices.size() ;
}

void DebugDrawer::addVertex(const Vec3 pos) {
  Vec4 col(1.0f);
  addVertex(pos, col);
}

void DebugDrawer::addVertex(const Vec3 pos, const Vec4 col) {
  needs_update = true;
  vertex.setPos(pos);
  vertex.setCol(col);
  vertices.push_back(vertex);
}

void DebugDrawer::addVertex(const float x, const float y, const float z, float r, float g, float b) {
  addVertex(Vec3(x,y,z), Vec4(r,g,b,1.0f));
}

void DebugDrawer::end() {
  entry.num_elements = vertices.size() - entry.start_dx;
  entries.push_back(entry);
}

void DebugDrawer::draw() {
  draw(ortho_matrix, view_matrix);
}

void DebugDrawer::draw(const float* pm, const float* vm) {
  if(!is_initialized) {
    setupOpenGL();
  }

  glPolygonMode(GL_FRONT_AND_BACK, must_fill ? GL_FILL : GL_LINE);

  glUseProgram(prog_id);
  glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, pm);
  glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, vm);
  glBindVertexArray(vao);

  if(needs_update) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    size_t needed = getNumBytes();
    if(needed > bytes_allocated) {
      while(bytes_allocated < needed) {
        bytes_allocated =  std::max<size_t>(bytes_allocated * 2, 256);
      }
      glBufferData(GL_ARRAY_BUFFER, bytes_allocated, NULL, GL_STREAM_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, needed, getPtr());
  }
 
  size_t dx = 0;
  for(std::vector<DrawEntry>::iterator it = entries.begin(); it != entries.end(); ++it) {
    DrawEntry& e = *it;
    glDrawArrays(e.type, dx, e.num_elements);
    dx += e.num_elements;
  }

  vertices.clear();
  entries.clear();
}

void DebugDrawer::drawTexture(GLuint tex, const float x, const float y, const float w, const float h) {
  if(!is_initialized) {
    setupOpenGL();
  }

  checkSize();
  tex_matrix[12] = x;
  tex_matrix[13] = y;
  tex_matrix[0] = w;
  tex_matrix[5] = h;

  glUseProgram(prog_tex);
  glUniformMatrix4fv(pm_id_tex, 1, GL_FALSE, ortho_matrix);
  glUniformMatrix4fv(mm_id_tex, 1, GL_FALSE, tex_matrix);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, tex);
  glUniform1i(tex_uniform, 3);

  glBindVertexArray(vao_tex);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
}

size_t DebugDrawer::getNumBytes() {
  return sizeof(VertexPC) * vertices.size();
}

const float* DebugDrawer::getPtr() {
  return vertices[0].getPtr();
}

void DebugDrawer::setFill(bool f) {
  must_fill = f;
}

void DebugDrawer::drawCircle(float x,  float y, float radius, bool filled, float r, float g, float b, float a) {
  begin((filled) ? GL_POLYGON : GL_LINE_LOOP);
  for(size_t i = 0; i < circle.size(); ++i) {
    VertexPC v = circle[i];
    Vec3 pos(x + v.pos.x * radius, y + v.pos.y * radius, 0.0f);
    Vec4 col(r, g, b, a);
    addVertex(pos, col);
  }
  end();
}

void DebugDrawer::drawRectangle(float x, float y, float w, float h, bool filled, float r, float g, float b, float a) {
  begin((filled) ? GL_POLYGON : GL_LINE_LOOP);
  addVertex(x, y, 0, r, g, b);         // top left
  addVertex(x + w, y, 0, r, g, b);     // top right 
  addVertex(x + w, y + h, 0, r, g, b); // bottom right
  addVertex(x, y + h, 0, r, g, b); // bottom left
  end();
}
#endif // ROXLU_WITH_OPENGL
