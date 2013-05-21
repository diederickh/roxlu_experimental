#include <glr/Camera.h>
#include <glr/Drawer.h>

namespace gl {

  Camera::Camera() 
    :screen_w(0)
    ,screen_h(0)
  {
  }

  Camera::~Camera() {
  }

  void Camera::place() {
    glr_set_view_matrix(vm().getPtr());
    glr_set_projection_matrix(pm().getPtr());
  }

  void Camera::setPosition(const float x, const float y, const float z) {
    position.set(x,y,z);
    update();
  }

  // fov = degrees
  void Camera::setPerspective(const float fov, const float aspect, const float n, const float f) {
    projection_matrix.perspective(fov, aspect, n, f);
  }

  void Camera::setupArcball(int w, int h) {
    screen_w = w;
    screen_h = h;
    half_screen_w = w * 0.5f;
    half_screen_h = h * 0.5f;
    setPerspective(60.0f , float(w)/h, 0.01, 600.0f);
  }

  void Camera::onMouseDown(int x, int y) {
    pstart = getSpherePoint(x, y);
    qstart = qnow;
  }

  Vec3 Camera::getSpherePoint(float x, float y) {
    Vec3 to( -1.0 + ((x/screen_w) * 2.0f), -(-1.0 + ((y/screen_h) * 2.0)), 0.0);
    float l = to.x * to.x + to.y * to.y;
    if(l > 1.0f) {
      float s = 1.0f / sqrt(l);
      to.x *= s;
      to.y *= s;
      to.z = 0.0f;
    }
    else {
      to.z = sqrt(1.0f - l);
    }
    return to;
  }

  void Camera::onMouseDrag(int x, int y) {
    Vec3 pnow = getSpherePoint(x,y);
    axis = cross(pstart, pnow);

    Quat nq;
    nq.x = axis.x;
    nq.y = axis.y;
    nq.z = axis.z;
    nq.w = dot(pstart, pnow);

    qnow = nq * qstart;

    Mat4 m;
    qnow.toMat4(m);
    rotation = qnow;
    update();
  }

  void Camera::update() {
    Mat4 trans;
    Mat3 rot3; 
    Mat4 rot4;
    rotation.toMat3(rot3);
    rot3.transpose();
    rot4 = rot3;
    trans.setPosition(-position.x, -position.y, -position.z);
    view_matrix = trans * rot4;
  }

}
