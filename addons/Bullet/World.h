#ifndef ROXLU_BULLET_WORLDH
#define ROXLU_BULLET_WORLDH

#include "bullet/btBulletDynamicsCommon.h"
#include "body/Body.h"
#include "body/Sphere.h"
#include "body/StaticPlane.h"
#include "body/Box.h"

//#include "shape/Plane.h"
#include "Roxlu.h"
#include "Constants.h"
#include "DebugDraw.h"

namespace roxlu {
namespace bullet {

namespace rb = roxlu::bullet;
namespace rbb = roxlu::bullet::body;

class World {
public:
	World();
	void create();
	void setGravity(float x, float y, float z);
	void update();
	void debugDraw();
	
	btBoxShape* createBoxShape(float sizeX, float sizeY, float sizeZ);
	rbb::Box* createBoxBody(btBoxShape* boxShape, const Vec3& position, const float mass);
	
	btStaticPlaneShape* createStaticPlaneShape(const Vec3& planeNormal, float planeConstant);
	rbb::StaticPlane* createStaticPlaneBody(btStaticPlaneShape* planeShape, const Vec3& position);
	btSphereShape* sphereCreateShape(float radius);
	rbb::Sphere* sphereCreateBody(btSphereShape* sphereShape, const Vec3& position, const float mass);


	static btTransform createTransform(const Vec3& position);
	static void setupBody(rbb::Body* body, btCollisionShape* collisionShape, const btTransform& transform, const float mass);
	btDiscreteDynamicsWorld* getBulletWorld();
private:
	
	btBroadphaseInterface* broadphase;		
	btDefaultCollisionConfiguration* config;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;
	rb::DebugDraw* debug_drawer; 
};

inline btDiscreteDynamicsWorld* World::getBulletWorld() {
	return world;
}

}} // roxlu::bullet

#endif