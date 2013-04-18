#include <experimental/Filter.h>
#include <experimental/Compositor.h>

namespace gl {

  Filter::Filter() { 
  }

  Filter::~Filter() {
  }


  bool Filter::getRenderPassSource(GLenum shader, std::string& global, std::string& main) {
    return false;
  }

  bool Filter::getShaderPassSource(GLenum shader, int pass, std::string& global, std::string& main) {
    return false;
  }

  bool Filter::createDrawBuffers(Compositor& com) {
    return false;
  }
  
  void Filter::addNode(SceneNode<VertexNP>* mesh) {
    nodes_np.push_back(mesh);
  }

  void Filter::render(Pass& pass) {
  }

  // ------------------------------------------------------------------------
  int FilterDiffuse::getNumShaderPasses() {
    return 0;
  }

  bool FilterDiffuse::getRenderPassSource(GLenum shader, std::string& global, std::string& main) {
    if(shader == GL_FRAGMENT_SHADER) {
      main = ""
        "gl_FragData[0].rgb = v_norm.rgb;  \n"
        "gl_FragData[0].a = 1.0;           \n";
      return true;
    }
    return false;
  }

  // ------------------------------------------------------------------------

  int FilterOcclusion::getNumShaderPasses() {
    return 1;
  }

  void FilterOcclusion::addLight(SceneNode<VertexNP>* mesh) {
    lights_np.push_back(mesh);
  }

  void FilterOcclusion::addOccluder(SceneNode<VertexNP>* mesh) {
    occluders_np.push_back(mesh);
  }

  bool FilterOcclusion::getRenderPassSource(GLenum shader, std::string& global, std::string& main) {

    if(shader == GL_FRAGMENT_SHADER) {
      main += occlusion_fragdata +" = vec4(u_col.r, u_col.g, u_col.b, 1.0); \n";
      return true;

    }
    return false;

  }

  bool FilterOcclusion::createDrawBuffers(Compositor& com) {
    com.createDrawBuffer(occlusion_fragdata, occlusion_uniform);
    return true;
  }

  bool FilterOcclusion::getShaderPassSource(GLenum shader, int pass, std::string& global, std::string& main) {
    printf("--------- %d \n", pass);
    if(shader == GL_FRAGMENT_SHADER && pass == 1) {
      /*
        "uniform float u_exposure;             \n"
        "uniform float u_decay;                   \n"
        "uniform float u_density;                \n" 
        "uniform float u_weight;                 \n"


       */
      global = ""
        "uniform sampler2D " +occlusion_uniform +";      \n"
        "const int NUM_SAMPLES = 50;                     \n"
        "";

      main = ""
        "float u_exposure = 0.0034; \n"
        "float u_decay = 1.0; \n"
        "float u_density = 0.84;\n"
        "float u_weight = 5.65; \n"
        "\n"
        "vec2 light_pos = vec2(0.5, 0.5);                \n"
        "vec2 tex_coord = v_tex;                         \n"
        "vec2 delta_tex = v_tex - light_pos;             \n"
        "delta_tex = delta_tex * (1.0 / float(NUM_SAMPLES)) * u_density; \n"
        "\n"
        "float illum_decay = 1.0;                        \n"
        "vec4 sample;                                    \n"
        "gl_FragColor = vec4(0);                         \n"
        "for(int i = 0; i < NUM_SAMPLES; ++i) {          \n"
        "  tex_coord -= delta_tex;                       \n"
        "  sample = texture2D(" +occlusion_uniform +", tex_coord);  \n"
        "  sample *= illum_decay * u_weight;             \n"
        "  gl_FragColor += sample;                       \n"
        "  gl_FragColor.r += 0.1;                        \n"
        "  gl_FragColor.g += 0.1;                        \n"
        "  illum_decay *= u_decay;                       \n"
        "}                                               \n"
        "\n"
        "gl_FragColor *= u_exposure;                     \n"
        "gl_FragColor += texture2D(u_tex1, v_tex) * 0.2;       \n"
        "gl_FragColor += texture2D(u_tex0, v_tex) * 0.2;       \n"
        "gl_FragColor.r += 0.2; \n"
        "gl_FragColor.a = 1.0; \n"
        "";

      //        "gl_FragColor = texture2D(" +occlusion_uniform +", v_tex); gl_FragColor.r += 0.2;\n";
  
      return true;
    }
    return false;
  }

  void FilterOcclusion::render(Pass& pass) {
    if(pass.num == 0) {

      Vec4 light_col(1.0, 1.0, 1.0, 1.0);
      Vec4 occluded_col(0.4, 0.2, 0.4, 1.0);

      glUniform4fv(pass.u_col, 1, light_col.getPtr());
      render(pass, lights_np);

      glUniform4fv(pass.u_col, 1, occluded_col.getPtr());
      render(pass, occluders_np);
    }
  }

  void FilterOcclusion::render(Pass& pass, std::vector<SceneNode<VertexNP>* >& nodes) {
    for(std::vector<SceneNode<VertexNP>* >::iterator it = nodes.begin(); it != nodes.end(); ++it) {
      SceneNode<VertexNP>* node = *it;
      node->mesh->bind();
      node->mesh->update();
      
      pass.setModelMatrix(node->mm().getPtr());
      glDrawArrays(GL_TRIANGLES, 0, node->mesh->size());
    }
  }

} // gl
