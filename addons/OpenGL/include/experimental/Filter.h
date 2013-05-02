#ifndef ROXLU_OPENGL_FILTER_H
#define ROXLU_OPENGL_FILTER_H

#include <string>
#include <vector>

#include <glr/Vertex.h>
#include <glr/Mesh.h>
#include <glr/FBO.h>
#include <experimental/Compositor.h>
#include <experimental/Scene.h>

namespace gl {

  // ----------------------------------------------------------
  class Compositor;
  class Pass;

  class Filter {

  public:
    Filter();
    ~Filter();
    void addNode(SceneNode<VertexNP>* node);

    virtual bool createDrawBuffers(Compositor& com); // returns false if no draw buffers are needed

    virtual int getNumShaderPasses() = 0;   // get number of full screen quad render buffers are needed
    virtual bool getRenderPassSource(GLenum shader, std::string& global, std::string& main); /* get the shader source for the render pass which must generate the draw buffers like motion buffer, depth buffer, occlusion buffer etc.. */
    virtual bool getShaderPassSource(GLenum shader, int pass, std::string& global, std::string& main);  /* get shader source for a full screen shader pass */

    virtual void render(Pass& pass);

  public:
    std::vector<SceneNode<VertexNP>* > nodes_np;
  };

  // ----------------------------------------------------------
  class FilterDiffuse : public Filter {
    int getNumShaderPasses();
    bool getRenderPassSource(GLenum shader, std::string& global, std::string& main); 
  };

  // ----------------------------------------------------------

  class FilterOcclusion : public Filter {

  public:
    bool createDrawBuffers(Compositor& com);

    int getNumShaderPasses();
    bool getRenderPassSource(GLenum shader, std::string& global, std::string& main);
    bool getShaderPassSource(GLenum shader, int pass, std::string& global, std::string& main); 


    void addLight(SceneNode<VertexNP>* mesh);
    void addOccluder(SceneNode<VertexNP>* mesh);
    void render(Pass& pass); 
    void render(Pass& pass, std::vector<SceneNode<VertexNP>* >& nodes);

  public:
    std::string occlusion_fragdata;
    std::string occlusion_uniform;
    std::vector<SceneNode<VertexNP>* > lights_np;
    std::vector<SceneNode<VertexNP>* > occluders_np;
  };

  /*

  class FilterBlurVertical : public Filter {
  public:
    int getNumShaderPasses();
    bool getShaderPassSource(GLenum shader, int pass, std::string& global, std::string& main);
  public:
  };
  */

} // gl

#if 0
    virtual bool gettVertexShaderSource(int pass, std::string& global, std::string& main);          /* get vertex shader source for 'outside' main() and inside main(), return true if we need to append any source */
    virtual bool getFragmentShaderSource(int pass, std::string& global, std::string& main);        /* get fragment shader source for 'outside' main() and inside main(), return true if we need to append any source */

  bool Filter::getVertexShaderSource(int pass, std::string& global, std::string& main) {
    return false;
  }

  bool Filter::getFragmentShaderSource(int pass, std::string& global, std::string& main) {
    return false;
  }

--
    bool getFragmentShaderSource(int pass, std::string& global, std::string& main);               /* get fragment shader source for 'outside' main() and inside main(), return true if we need to append any source */

  bool FilterOcclusion::getFragmentShaderSource(int pass, std::string& global, std::string& main) {

    if(pass == 0) {
      main += ""
        " gl_FragColor.rgb = u_col.rgb; \n"
        " gl_FragColor.a = 1.0;\n ";
      return true;
    }

    return false;
  }

#endif

#endif
