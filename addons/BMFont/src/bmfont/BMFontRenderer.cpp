#include <bmfont/BMFontLoader.h>
#include <bmfont/BMFontRenderer.h>

GLuint BMFontRenderer::prog = 0;
GLuint BMFontRenderer::vao = 0;
GLint BMFontRenderer::u_projection_matrix = -1;
GLint BMFontRenderer::u_tex = -1;


BMFontRenderer::BMFontRenderer(BMFontLoader& font)
  :font(font)
  ,is_setup(false)
{
  clear();
  
  if(!prog) {
    prog = rx_create_shader(BMFONT_VS, BMFONT_FS);
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");
    glBindAttribLocation(prog, 2, "a_fg_color");
    glLinkProgram(prog);
    
    glUseProgram(prog);
    u_projection_matrix = glGetUniformLocation(prog, "u_projection_matrix");
    u_tex = glGetUniformLocation(prog, "u_tex");

    glGenVertexArrays(1, &vao);
  }

  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BVertex), (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BVertex), (GLvoid*)8);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BVertex), (GLvoid*)16);
  
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

BMFontRenderer::~BMFontRenderer() {
  if(vbo) {
    glDeleteBuffers(1, &vbo);
  }
  
  if(tex) {
    glDeleteTextures(1, &tex);
  }

  clear();
  
}

void BMFontRenderer::clear() {
  vbo = 0;
  tex = 0;
  u_projection_matrix = -1;
  u_tex = -1;
  bytes_allocated = 0;
  is_setup = false;

  memset(projection_matrix, 0x00, sizeof(projection_matrix));

  multi_firsts.clear();
  multi_counts.clear();
  vertices.clear();
}

void BMFontRenderer::reset() {
  vertices.clear();
  multi_firsts.clear();
  multi_counts.clear();
}

void BMFontRenderer::setup(int windowW, int windowH) {
  rx_ortho(0, windowW, windowH, 0.0, -1.0, 1.0, projection_matrix);

  if(!font.getImageWidth() || !font.getImageHeight() || !font.getImagePath().size()) {
    RX_ERROR((BMF_ERR_NO_IMAGE));
    ::exit(EXIT_FAILURE);
  }

  Image img;
  if(!img.load(font.getImagePath())) {
    RX_ERROR((BMF_ERR_NO_IMAGE_FOUND));
    ::exit(EXIT_FAILURE);
  }

  if(img.getComponents() != 1) {
    RX_ERROR((BMF_ERR_WRONG_NCOMPONENTS));
    ::exit(EXIT_FAILURE);
  }

  if(img.getWidth() != font.getImageWidth() || img.getHeight() != font.getImageHeight()) {
    RX_ERROR((BMF_ERR_IMAGE_SIZE));
    ::exit(EXIT_FAILURE);
  }

  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, img.getWidth(), img.getHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, img.getPixels());
  is_setup = true;
}

void BMFontRenderer::addVertices(std::vector<BVertex>& in) {
  multi_firsts.push_back(vertices.size());
  multi_counts.push_back(in.size());

  std::copy(in.begin(), in.end(), std::back_inserter(vertices));
}

void BMFontRenderer::update() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  size_t bytes_needed = vertices.size() * sizeof(BVertex);
  if(bytes_needed > bytes_allocated) {
    while(bytes_allocated < bytes_needed) {
      bytes_allocated = std::max<size_t>(bytes_allocated * 2, 1024);
    }
    glBufferData(GL_ARRAY_BUFFER, bytes_allocated, NULL, GL_STREAM_DRAW);
  }

  glBufferSubData(GL_ARRAY_BUFFER, 0, bytes_needed, vertices[0].getPtr());
}


void BMFontRenderer::draw() {
  if(!is_setup) {
    RX_ERROR((BMF_ERR_NOT_SETUP));
    return;
  }
  if(!bytes_allocated) { 
    RX_ERROR((BMF_ERR_NOT_ALLOC));
    return ;
  }
  if(!vertices.size()) {
    return ;
  }
  
  glBindVertexArray(vao);
  glUseProgram(prog);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glUniform1i(u_tex, 0);

  glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE, projection_matrix);

#if defined(ROXLU_GL_CORE3)
  glMultiDrawArrays(GL_TRIANGLES, &multi_firsts[0], &multi_counts[0], multi_counts.size());
#else 
  for(size_t i = 0; i < multi_firsts.size(); ++i) {
    glDrawArrays(GL_TRIANGLES, multi_firsts[i], multi_counts[i]);
  }
#endif

}
