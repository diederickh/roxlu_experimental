#include <experimental/filters/VerticalBlur.h>
#include <experimental/Compositor.h>
#include <roxlu/core/Log.h>

namespace gl { 

  VerticalBlur::VerticalBlur(float amount) 
    :blur_amount(amount)
  {
  }

  bool VerticalBlur::setup(int w, int h) { 

    // CREATE SHADER
    if(!shader.create(VERTICAL_BLUR_FILTER_VS, VERTICAL_BLUR_FILTER_FS)) {
      RX_ERROR(ERR_GL_VBLUR_SHADER);
      return false;
    }

    shader.bindAttribLocation("a_pos", 0);
    shader.bindAttribLocation("a_tex", 1);

    if(!shader.link()) {
      RX_ERROR(ERR_GL_VBLUR_SHADER);
      return false;
    }

    shader.use();
    u_tex = shader.getUniformLocation("u_tex");
    u_height = shader.getUniformLocation("u_height");

    glUniform1f(u_height, h); 


    // SET BLUR VALUES
    const int num_weights = 10;
    char weight_uniform_name[50];
    char offset_uniform_name[50];
    float weights[num_weights];
    float sum;
    float sigma2 = blur_amount;

    weights[0] = gauss(0.0f, sigma2);
    sum = weights[0];
    for(int i = 1; i < num_weights; ++i) {
      weights[i] = gauss(i, sigma2);
      sum += 2 * weights[i];
    }

    for(int i = 0; i < num_weights; ++i) {
      sprintf(weight_uniform_name, "u_weight[%d]", i);
      GLint weight_uniform_loc = glGetUniformLocation(shader.prog, weight_uniform_name);
      float val = weights[i] / sum;
      glUniform1f(weight_uniform_loc, val);

      sprintf(offset_uniform_name, "u_pix_offset[%d]", i);
      GLint offset_uniform_loc = glGetUniformLocation(shader.prog, offset_uniform_name);
      glUniform1f(offset_uniform_loc, 2.5f * i);

      //printf("> weight: %s, %d offset: %s, %d, value: %f\n", weight_uniform_name, weight_uniform_loc, offset_uniform_name, offset_uniform_loc, val);
    }

    return true;
  }

  float VerticalBlur::gauss(float x, float sigma2) {
    double coeff = 1.0 / (2.0 * PI * sigma2);
    double expon = -(x * x) / (2.0 * sigma2);
    return (float) (coeff * exp(expon));
  }

  void VerticalBlur::render() { 
    shader.use();

    glDrawBuffer(output.attachment);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, input.texture);
    glUniform1i(u_tex, 0);
  }

} // gl

