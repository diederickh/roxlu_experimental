#include <roxlu/opengl/GL.h>
#include <bmfont/BMFShader.h>

GLuint BMFShader::prog = 0;
GLint BMFShader::u_projection_matrix = -1;
GLint BMFShader::u_model_matrix = -1;
GLint BMFShader::u_tex = -1;
GLint BMFShader::u_alpha = -1;
GLuint BMFShader::u_vert_shader = 0;
GLuint BMFShader::u_frag_shader = 0;

BMFShader::BMFShader()
  :tex(0)
{
}

BMFShader::~BMFShader() {
  tex = 0;
}


void BMFShader::setup(GLuint vbo, GLuint texture) {
  if(!prog) {
    prog = rx_create_shader(BMFONT_VS, BMFONT_FS, u_vert_shader, u_frag_shader);
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");
    glBindAttribLocation(prog, 2, "a_fg_color");
    glLinkProgram(prog);
    
    glUseProgram(prog);
    u_projection_matrix = glGetUniformLocation(prog, "u_projection_matrix");
    u_model_matrix = glGetUniformLocation(prog, "u_model_matrix");
    u_tex = glGetUniformLocation(prog, "u_tex");
    u_alpha = glGetUniformLocation(prog, "u_alpha");
    
    assert(u_model_matrix >= 0);
    assert(u_projection_matrix >= 0);
    assert(u_tex >= 0);

    glUniform1f(u_alpha, 1.0f);
    glUseProgram(0);
  }

  // somehow the macro mapping in roxlu/opengl/GL.h does not work correctly - do not remove this! :-) 
#if defined(ROXLU_GL_CORE3)
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
#else
  glGenVertexArraysAPPLE(1, &vao);
  glBindVertexArrayAPPLE(vao);
#endif

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BMFVertex), (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BMFVertex), (GLvoid*)8);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BMFVertex), (GLvoid*)16);
  tex = texture;

#if defined(ROXLU_GL_CORE3)
  glBindVertexArray(0);
#else
  glBindVertexArrayAPPLE(0);
#endif

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


