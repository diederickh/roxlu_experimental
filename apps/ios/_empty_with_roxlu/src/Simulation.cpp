#include "Simulation.h"


Simulation::Simulation() {
}

Simulation::~Simulation() {
}

void Simulation::setup() {

  printf("Simulation::setup()\n");
  // CREATE SHADER
  // --------------
  GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  const char* vss = TEST_VS.c_str();
  const char* fss = TEST_FS.c_str();
  glShaderSource(vert_id, 1, &vss, NULL);
  glShaderSource(frag_id, 1, &fss, NULL);
  glCompileShader(vert_id);
  glCompileShader(frag_id);
  prog = glCreateProgram();
  glAttachShader(prog, vert_id);
  glAttachShader(prog, frag_id);
  glLinkProgram(prog);
              

  // CREATE VAO/VBO
  glGenVertexArraysOES(1, &vao);
  glBindVertexArrayOES(vao);

  float s = 0.5f;
  float vertices[] = {
    -s, -s, 
    s, -s,
    s, s,

    -s, -s, 
    s, s, 
    -s, s
  };
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glUseProgram(prog);
  glEnableVertexAttribArray(glGetAttribLocation(prog, "a_pos"));
  glVertexAttribPointer(glGetAttribLocation(prog, "a_pos"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)0);

}

void Simulation::update() {
}

void Simulation::draw() {
  glUseProgram(prog);
  glBindVertexArrayOES(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
