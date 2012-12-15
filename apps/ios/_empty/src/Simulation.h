#ifndef ROXLU_SIMULATION_H
#define ROXLU_SIMULATION_H


#include <string>
#include <iostream>
#include <stdio.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

const std::string TEST_VS = ""
  "attribute vec4 a_pos;"
  "void main() {"
  "   gl_Position = a_pos; "
  "}";

const std::string TEST_FS = ""
  "void main() { "
  "   gl_FragColor = vec4(1.0, 0.0, 0.4, 1.0);"
  "}";
  
class Simulation {
 public:
  Simulation();
  ~Simulation();
  void setup();
  void update();
  void draw();
 private:
  GLuint prog;
  GLuint vao;
  GLuint vbo;
};

#endif
