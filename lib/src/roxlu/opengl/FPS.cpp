#include <roxlu/opengl/FPS.h>

namespace roxlu {
  FPS_Element::FPS_Element()
    :start(0)
    ,num_vertices(0)
  {
  }

  void FPS_Element::set(unsigned int s, unsigned int num) {
    start = s;
    num_vertices = num;
  }

  FPS::FPS(int align, float scale)
    :timeout(0)
    ,frame_count(0)
    ,is_setup(false)
    ,win_w(0)
    ,win_h(0)
    ,align(align)
    ,scale(scale)
    ,x(0)
    ,y(0)
  {
    memset(pm, 0, sizeof(float) * 16);
    memset(mm, 0, sizeof(float) * 16);
    memset(draw_list, 0, sizeof(int) * 4);
    mm[0] = mm[5] = mm[10] = mm[15] = 1.0f;
    fg_col[0] = 1.0f; 
    fg_col[1] = 1.0f; 
    fg_col[2] = 0.0f; 
    fg_col[3] = 1.0f; 
  }

  FPS::~FPS() {
  }

  void FPS::draw() {
    if(!is_setup) {
      setupGL();
    }

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    if(vp[2] != win_w || vp[3] != win_h) {
      win_w = vp[2];
      win_h = vp[3];
      createOrtho(win_w, win_h);

      if(align == FA_BOTTOM_RIGHT) {
        x = (win_w) - (4 * (scale * 6.5));
        y = win_h - (7.0f * scale) - 5.0f;
      }
      else if(align == FA_TOP_RIGHT) {
        x = (win_w) - (4 * (scale * 6.5));
        y = 5.0f;
      }
      else if(align == FA_TOP_LEFT) {
        x = 5.0f;
        y = 5.0f;
      }
      else if(align == FA_BOTTOM_LEFT) {
        x = 5.0f;
        y = win_h - (7.0f * scale) - 5.0f;
      }
    }

    rx_uint64 now = Timer::now();
    ++frame_count;
    if(now > timeout) {
      timeout = now + 1000;
      fps = frame_count;
      frame_count = 0;
      unsigned char buf[256];
      sprintf((char*)buf, "%04d", fps);
      if(strlen((char*)buf) == 4) {
        for(int i = 0; i < 4; ++i) {
          unsigned char c = buf[i] - '0';
          draw_list[i] = c;
        }
      }
    }

    vao.bind();
    shader.enable();
    shader.uniformMat4fv("u_pm", pm);
  
    mm[12] = x;
    mm[13] = y;
    mm[14] = -0.4f;
    mm[0] = mm[5] = mm[10] = scale;

    glLineWidth(2.0f);
    shader.uniform4fv("u_col", fg_col);
    shader.uniformMat4fv("u_mm", mm);
    for(int i = 0; i < 4; ++i) {
      mm[12] = x + i * (scale*6.5);
      shader.uniformMat4fv("u_mm", mm);
      unsigned int dx = draw_list[i];
      glDrawArrays(GL_LINES, elements[dx].start, elements[dx].num_vertices);
    }
  }

  void FPS::setupGL() {
    is_setup = true;
  
    // SHADER
    shader.create(FPS_VS, FPS_FS);
    shader.a("a_pos", 0);
    shader.link();
    shader.u("u_mm").u("u_pm").u("u_col");

    // VBO + VAO
    elements[0].set(0,10);
    elements[1].set(10,4);
    elements[2].set(14,10);
    elements[3].set(24,8);
    elements[4].set(32,6);
    elements[5].set(38,10);
    elements[6].set(48,10);
    elements[7].set(58,5);
    elements[8].set(64,10);
    elements[9].set(74,10);

    float vertices[] = {
      // 0 (0, 10)
      0.0f, 0.0f, 4.0f, 0.0f,
      4.0f, 0.0f, 4.0f, 7.0f,
      4.0f, 7.0f, 0.0f, 7.0f,
      0.0f, 7.0f, 0.0f, 0.0f, // circle round
      0.0f, 0.0f, 4.0f, 7.0f, // diagonal line

      // 1 (10, 4)
      0.0f, 3.0f, 3.0f, 0.0f, 
      3.0f, 0.0f, 3.0f, 7.0f,

      // 2, (14, 10)
      0.0f, 0.0f, 4.0f, 0.0f,
      4.0f, 0.0f, 4.0f, 4.0f,
      4.0f, 4.0f, 0.0f, 4.0f,
      0.0f, 4.0f, 0.0f, 7.0f,
      0.0f, 7.0f, 4.0f, 7.0f,
    
      // 3 (24, 8)
      0.0f, 0.0f, 4.0f, 0.0f,
      4.0f, 0.0f, 4.0f, 7.0f,
      4.0f, 4.0f, 1.0f, 4.0f,
      0.0f, 7.0f, 4.0f, 7.0f,

      // 4 (32, 6)
      0.0f, 0.0f, 0.0f, 4.0f,
      0.0f, 4.0f, 4.0f, 4.0f,
      4.0f, 0.0f, 4.0f, 7.0f,
    
      // 5 (38, 10)
      0.0f, 0.0f, 4.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 4.0f,
      0.0f, 4.0f, 4.0f, 4.0f,
      4.0f, 4.0f, 4.0f, 7.0f,
      0.0f, 7.0f, 4.0f, 7.0f,

      // 6 (48, 10)
      0.0f, 0.0f, 0.0f, 7.0f,
      0.0f, 0.0f, 4.0f, 0.0f,
      0.0f, 4.0f, 4.0f, 4.0f,
      0.0f, 7.0f, 4.0f, 7.0f,
      4.0f, 4.0f, 4.0f, 7.0f,

      // 7 (58, 6)
      0.0f, 0.0f, 4.0f, 0.0f,
      4.0f, 0.0f, 4.0f, 7.0f,
      1.0f, 4.0f, 4.0f, 4.0f,

      // 8 (64, 10)
      0.0f, 0.0f, 0.0f, 7.0f,
      4.0f, 0.0f, 4.0f, 7.0f,
      0.0f, 0.0f, 4.0f, 0.0f,
      0.0f, 7.0f, 4.0f, 7.0f,
      0.0f, 4.0f, 4.0f, 4.0f,

      // 9 (74, 10)
      0.0f, 0.0f, 0.0f, 4.0f,
      4.0f, 0.0f, 4.0f, 7.0f,
      0.0f, 0.0f, 4.0f, 0.0f,
      0.0f, 7.0f, 4.0f, 7.0f,
      0.0f, 4.0f, 4.0f, 4.0f,
    };

    vao.bind();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // a_pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)0);
  }

  void FPS::createOrtho(int winWidth, int winHeight) {
    float n = 0.0;
    float f = 1.0;
    float ww = winWidth;
    float hh = winHeight;
    float fmn = f - n;
    pm[15] = 1.0f;
    pm[0]  = 2.0f / ww;
    pm[5]  = 2.0f / -hh;
    pm[10] = -2.0f / fmn;
    pm[12] = -(ww)/ww;
    pm[13] = -(hh)/-hh;
    pm[14] = -(f+n)/fmn;
  }
}
