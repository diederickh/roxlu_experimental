#ifndef ROXLU_OPENGL_CAMERA_H
#define ROXLU_OPENGL_CAMERA_H

#include <roxlu/math/Math.h>

namespace gl {

  struct Camera {
    Camera();
    ~Camera();

    // arcball: gives Camera rotation! so we need to invert to get view_matrix!
    void setupArcball(int screenW, int screenH); // based on https://bitbucket.org/dbacchet/dbsdev/src/8a68c9ab4c87/docs/papers/Shoemake%20-%201992%20-%20Arcball.pdf
    void onMouseDown(int x, int y);
    void onMouseDrag(int x, int y);
    Vec3 getSpherePoint(float x, float y);

    // cam
    void place();
    void setPosition(const float x, const float y, const float z);
    void setPerspective(const float fov, const float aspect, const float n, const float f); // fov = degrees
    void update();
    Mat4& pm();
    Mat4& vm();

    Mat4 projection_matrix;
    Mat4 view_matrix;
    Quat rotation;
    Vec3 position;

    // arcball
    float screen_w;
    float screen_h;
    float half_screen_w;
    float half_screen_h;
    Vec3 pstart;
    Vec3 axis;
    Quat qstart;
    Quat qnow;
  };

  inline Mat4& Camera::pm() {
    return projection_matrix;
  }

  inline Mat4& Camera::vm() {
    return view_matrix;
  }

} // gl 
#endif
