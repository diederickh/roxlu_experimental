#ifndef ROXLU_BULLET_SHAPE_SPHEREH
#define ROXLU_BULLET_SHAPE_SPHEREH

#include "bullet/btBulletDynamicsCommon.h"
#include "Body.h"

namespace roxlu {
namespace bullet {
namespace body {

class Sphere : public Body {
public:
	Sphere();
	~Sphere();
};

}}} // roxlu::bullet::body
#endif