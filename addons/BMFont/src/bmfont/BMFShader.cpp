#include <roxlu/opengl/GL.h>
#include <bmfont/BMFShader.h>

GLuint BMFShader::prog = 0;
GLint BMFShader::u_projection_matrix = -1;
GLint BMFShader::u_model_matrix = -1;
GLint BMFShader::u_tex = -1;
GLint BMFShader::u_alpha = -1;

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
    u_model_matrix = glGetUniformLocation(prog, "u_model_matrix");
    u_tex = glGetUniformLocation(prog, "u_tex");
    u_alpha = glGetUniformLocation(prog, "u_alpha");
    
    glUniform1f(u_alpha, 1.0f);
  }

  glGenVertexArrays(1, &vao);
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

void BMFShader::draw() {
  glBindVertexArray(vao);
  glUseProgram(prog);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glUniform1i(u_tex, 0);
}

