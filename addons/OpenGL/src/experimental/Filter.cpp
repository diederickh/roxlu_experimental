#include <experimental/Filter.h>
#include <experimental/RenderPass.h>

namespace gl {

  Filter::Filter() { 
  }

  Filter::~Filter() {
  }

  bool Filter::getVertexShaderSource(std::string& global, std::string& main) {
    return false;
  }

  bool Filter::getFragmentShaderSource(std::string& global, std::string& main) {
    return false;
  }
  
  void Filter::addNode(SceneNode<VertexNP>* mesh) {
    nodes_np.push_back(mesh);
  }

  void Filter::render(RenderPass& pass) {
  }

  // ------------------------------------------------------------------------

  void FilterOcclusion::addLight(SceneNode<VertexNP>* mesh) {
    lights_np.push_back(mesh);
  }

  void FilterOcclusion::addOccluder(SceneNode<VertexNP>* mesh) {
    occluders_np.push_back(mesh);
  }

  bool FilterOcclusion::getFragmentShaderSource(std::string& global, std::string& main) {
    main += ""
      " gl_FragColor.rgb = u_col.rgb; \n"
      " gl_FragColor.a = 1.0;\n ";
    return true;
  }

  void FilterOcclusion::render(RenderPass& pass) {
    Vec4 light_col(1.0, 1.0, 1.0, 1.0);
    Vec4 occluded_col(0.1, 0.25, 0.1, 1.0);

    glUniform4fv(pass.u_col, 1, light_col.getPtr());
    render(pass, lights_np);

    glUniform4fv(pass.u_col, 1, occluded_col.getPtr());
    render(pass, occluders_np);
  }

  void FilterOcclusion::render(RenderPass& pass, std::vector<SceneNode<VertexNP>* >& nodes) {
    for(std::vector<SceneNode<VertexNP>* >::iterator it = nodes.begin(); it != nodes.end(); ++it) {
      SceneNode<VertexNP>* node = *it;
      node->mesh->bind();
      node->mesh->update();
      pass.setModelMatrix(node->mm().getPtr());
      glDrawArrays(GL_TRIANGLES, 0, node->mesh->size());
    }
  }

} // gl
