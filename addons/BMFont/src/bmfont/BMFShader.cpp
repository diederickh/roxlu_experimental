#include <bmfont/BMFShader.h>

GLuint BMFShader::prog = 0;
GLuint BMFShader::vao = 0;
GLint BMFShader::u_projection_matrix = -1;
GLint BMFShader::u_tex = -1;

BMFShader::BMFShader()
  :tex(0)
{
}

BMFShader::~BMFShader() {
  tex = 0;
}


void BMFShader::setup(GLuint vbo, GLuint tex) {
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
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BMFVertex), (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BMFVertex), (GLvoid*)8);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BMFVertex), (GLvoid*)16);

  this->tex = tex;
}

void BMFShader::draw(const float* projectionMatrix) {
  glBindVertexArray(vao);
  glUseProgram(prog);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glUniform1i(u_tex, 0);

  glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE, projectionMatrix);
}

