#ifndef ROXLU_OPENGL_FILTER_H
#define ROXLU_OPENGL_FILTER_H

#include <string>
#include <vector>

#include <glr/Vertex.h>
#include <glr/Mesh.h>
#include <experimental/RenderPass.h>
#include <experimental/Scene.h>

namespace gl {

  // ----------------------------------------------------------
  class RenderPass;

  class Filter {

  public:
    Filter();
    ~Filter();
    void addNode(SceneNode<VertexNP>* node);
    virtual bool getVertexShaderSource(std::string& global, std::string& main);          /* get vertex shader source for 'outside' main() and inside main(), return true if we need to append any source */
    virtual bool getFragmentShaderSource(std::string& global, std::string& main);        /* get fragment shader source for 'outside' main() and inside main(), return true if we need to append any source */
    virtual void render(RenderPass& pass);
  public:
    std::vector<SceneNode<VertexNP>* > nodes_np;
  };

  // ----------------------------------------------------------

  class FilterOcclusion : public Filter{

  public:
    bool getFragmentShaderSource(std::string& global, std::string& main);               /* get fragment shader source for 'outside' main() and inside main(), return true if we need to append any source */
    void addLight(SceneNode<VertexNP>* mesh);
    void addOccluder(SceneNode<VertexNP>* mesh);
    void render(RenderPass& pass);
    void render(RenderPass& pass, std::vector<SceneNode<VertexNP>* >& nodes);
  public:
    std::vector<SceneNode<VertexNP>* > lights_np;
    std::vector<SceneNode<VertexNP>* > occluders_np;
  };

} // gl
#endif
