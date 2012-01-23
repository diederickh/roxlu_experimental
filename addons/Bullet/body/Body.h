#ifndef ROXLU_BULLET_SHAPEH
#define ROXLU_BULLET_SHAPEH

#include "bullet/btBulletDynamicsCommon.h"

namespace roxlu {
namespace bullet {
namespace body {

class Body {
public:
	Body();
	~Body();
	void applyCentralForce(float x, float y, float z);
	btRigidBody* rigid_body;
	btCollisionShape* collision_shape;
	btMotionState* motion_state;
};

inline void Body::applyCentralForce(float x, float y, float z) {
	rigid_body->applyCentralForce(btVector3(x,y,z));
}

}}} // roxlu::bullet::shape
#endif