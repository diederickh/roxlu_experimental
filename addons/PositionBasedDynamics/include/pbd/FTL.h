#ifndef ROXLU_FTL_H
#define ROXLU_FTL_H

#include <roxlu/Roxlu.h>

namespace ftl {

  struct Particle {
    Particle(Vec3 position, float m);
    Vec3 position;
    Vec3 tmp_position;
    Vec3 forces;
    Vec3 velocity;
    Vec3 d;
    float mass;
    float inv_mass;
    bool enabled;

  };

  class FTL {
  public:
    FTL();
    void setup(int num, float d);
    void addForce(Vec3 f);
    void update();
    void draw();
  public:
    float len;
    std::vector<Particle*> particles;
    Vec3 color;
  };

}
#endif
