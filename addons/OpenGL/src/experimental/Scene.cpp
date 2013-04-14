#include <experimental/Scene.h>

namespace gl {

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::addNode(SceneNode<VertexNP>* node) {
  nodes_np.push_back(node);
}

void Scene::draw() {
  draw(nodes_np);
}

void Scene::draw(std::vector<SceneNode<VertexNP>* >& nodes) {
  Shader& shader_np = glr_get_shader_np();
  shader_np.use();

  for(std::vector<SceneNode<VertexNP>* >::iterator it = nodes.begin(); it != nodes.end(); ++it) {
    SceneNode<VertexNP>* node = *it;
    node->mesh->bind();
    node->mesh->update();

    shader_np.setModelMatrix(node->mm().getPtr());
    glDrawArrays(GL_TRIANGLES, 0, node->mesh->size());
  }

  shader_np.unuse();
}

} // gl
