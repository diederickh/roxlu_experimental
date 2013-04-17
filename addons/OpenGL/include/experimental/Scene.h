#ifndef ROXLU_OPENGL_SCENE_H
#define ROXLU_OPENGL_SCENE_H

#include <vector>
#include <assert.h>
#include <roxlu/math/Math.h>
#include <glr/Vertex.h>
#include <glr/Mesh.h>

namespace gl {


  // --------------------------------------------------

  template<class T>
    class SceneNode {

  public:
    SceneNode(Mesh<T>* mesh);
    void setPosition(Vec3 position);
    void setScale(float s);
    void draw();
    Mat4& mm();                              /* return the model matrix */

  public:
    Mat4 model_matrix;
    Mesh<T>* mesh;
  };

  // --------------------------------------------------

  class Scene {
  public:
    Scene();
    ~Scene();
    void addNode(SceneNode<VertexNP>* node);
    void draw();
    void draw(std::vector<SceneNode<VertexNP>* >& nodes);
  public:
    std::vector<SceneNode<VertexNP>* > nodes_np;
  };



  // --------------------------------------------------

  template<class T>
    inline SceneNode<T>::SceneNode(Mesh<T>* m):mesh(m)
    {
    }

  template<class T>
    inline void SceneNode<T>::setPosition(Vec3 position) {
    assert(mesh);
    model_matrix.setPosition(position);
  }

  template<class T>
    inline void SceneNode<T>::setScale(float s) {
    assert(mesh);
    model_matrix.setScale(s);
  }

  template<class T>
    inline void SceneNode<T>::draw() {
  }

  template<class T>
    inline Mat4& SceneNode<T>::mm() {
    return model_matrix;
    
  }

} // gl
#endif
