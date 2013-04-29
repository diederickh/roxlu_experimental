#include <glr/Font.h>

namespace gl {

void glr_draw_string(const ::std::string& str, float x, float y, float r, float g, float b) {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // to allocate
  size_t to_allocate = str.size() * sizeof(float) * 4 * 6; // 4-floats per vertex, 6 floats (two tris) per char
  if(to_allocate > glr_font.allocated) {
    while(glr_font.allocated < to_allocate) {
      glr_font.allocated = ::std::max<size_t>(glr_font.allocated * 2, 4096);
    }

    glBindVertexArray(glr_font.vao);
    glBindBuffer(GL_ARRAY_BUFFER, glr_font.vbo);
    glBufferData(GL_ARRAY_BUFFER, glr_font.allocated, NULL, GL_DYNAMIC_DRAW);

    delete[] glr_font.buffer; 
    glr_font.buffer = new float[glr_font.allocated];
    memset(glr_font.buffer, 0, glr_font.allocated);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);
  }

  size_t floats_per_vertex = 4; // 2-xy, 2-st
  size_t dx = 0;
  size_t char_count = 0;
  const char* ptr = str.c_str();

  for(int i = 0; i < str.size(); ++i) {

    int c = str[i];
    int dd = c - STB_SOMEFONT_FIRST_CHAR;

    stb_fontchar* cd = &glr_font.fontdata[c - STB_SOMEFONT_FIRST_CHAR];
    if(str[i] == ' ') {
      x += 5;
      continue;
    }

    glr_font.buffer[dx++] = x + cd->x0f;
    glr_font.buffer[dx++] = y + cd->y1f;
    glr_font.buffer[dx++] = cd->s0f;
    glr_font.buffer[dx++] = cd->t1f;

    glr_font.buffer[dx++] = x + cd->x1f;
    glr_font.buffer[dx++] = y + cd->y1f;
    glr_font.buffer[dx++] = cd->s1f;
    glr_font.buffer[dx++] = cd->t1f;

    glr_font.buffer[dx++] = x + cd->x1f;
    glr_font.buffer[dx++] = y + cd->y0f;
    glr_font.buffer[dx++] = cd->s1f;
    glr_font.buffer[dx++] = cd->t0f;

    // --
    glr_font.buffer[dx++] = x + cd->x0f;
    glr_font.buffer[dx++] = y + cd->y1f;
    glr_font.buffer[dx++] = cd->s0f;
    glr_font.buffer[dx++] = cd->t1f;

    glr_font.buffer[dx++] = x + cd->x1f;
    glr_font.buffer[dx++] = y + cd->y0f;
    glr_font.buffer[dx++] = cd->s1f;
    glr_font.buffer[dx++] = cd->t0f;

    glr_font.buffer[dx++] = x + cd->x0f;
    glr_font.buffer[dx++] = y + cd->y0f;
    glr_font.buffer[dx++] = cd->s0f;
    glr_font.buffer[dx++] = cd->t0f;

    x += cd->x1f;
    char_count++;
  }

  glUseProgram(glr_font.prog);
  glBindVertexArray(glr_font.vao);

  glBindBuffer(GL_ARRAY_BUFFER, glr_font.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, (char_count * 6 * 4 * sizeof(float)) , glr_font.buffer); 

  float col[3] = {r,g,b};
  glUniform3fv(glr_font.u_col, 1, col);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glr_font.tex);
  glUniform1i(glr_font.u_tex, 0);
  
  glUniformMatrix4fv(glr_font.u_pm, 1, GL_FALSE, glr_font.pm);
  glDrawArrays(GL_TRIANGLES, 0, char_count * 6);
}

  Font glr_font;

} // gl
