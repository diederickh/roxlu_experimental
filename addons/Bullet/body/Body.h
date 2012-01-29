#ifndef ROXLU_BULLET_SHAPEH
#define ROXLU_BULLET_SHAPEH

#include "bullet/btBulletDynamicsCommon.h"
#include "Mat4.h"
namespace roxlu {
namespace bullet {
namespace body {

class Body {
public:
	Body();
	~Body();
	void applyCentralForce(float x, float y, float z);
	virtual void copyMatrix(Mat4& m) const;
	
	btRigidBody* rigid_body;
	btCollisionShape* collision_shape;
	btDefaultMotionState* motion_state;
;
};

inline void Body::applyCentralForce(float x, float y, float z) {
	rigid_body->applyCentralForce(btVector3(x,y,z));
}

}}} // roxlu::bullet::shape
#endif