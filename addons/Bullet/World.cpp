#include "World.h"
#include "Vec3.h"
#include "Constants.h"

namespace roxlu {
namespace bullet {

namespace rb = roxlu::bullet;

World::World() 
	:broadphase(NULL)
	,config(NULL)
	,dispatcher(NULL)
	,solver(NULL)
	,world(NULL)
	,debug_drawer(NULL)
{
}

void World::create() {
	broadphase = new btDbvtBroadphase();
	config = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(config);
	solver = new btSequentialImpulseConstraintSolver;
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
	
	debug_drawer = new roxlu::bullet::DebugDraw();
	debug_drawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe /*| btIDebugDraw::DBG_DrawAabb*/);
	world->setDebugDrawer(debug_drawer);
}

void World::setGravity(float x, float y, float z) {
	world->setGravity(btVector3(x,y,z));	
}

void World::update() {
	world->stepSimulation(1/60.0f, 10);
}


// Sphere
// -----------------------------------------------------------------------------
btSphereShape* World::sphereCreateShape(float radius) {
	btSphereShape* s = new btSphereShape(radius);
	return s;
}

rbb::Sphere* World::sphereCreateBody(btSphereShape* sphereShape, const Vec3& position, const float mass ) {
	rbb::Sphere* sphere = new rbb::Sphere();
	World::setupBody(sphere, sphereShape, World::createTransform(position), mass);
	world->addRigidBody(sphere->rigid_body);
	return sphere;
}


// StaticPlane
// -----------------------------------------------------------------------------
btStaticPlaneShape* World::createStaticPlaneShape(const Vec3& planeNormal, float planeConstant) {
	btVector3 normal(planeNormal.x,planeNormal.y,planeNormal.z);
	btStaticPlaneShape* s = new btStaticPlaneShape(normal, planeConstant);
	return s;
}

rbb::StaticPlane* World::createStaticPlaneBody(btStaticPlaneShape* planeShape, const Vec3& position) {
	rbb::StaticPlane* plane = new rbb::StaticPlane();	
	World::setupBody(plane, planeShape, World::createTransform(position),0);
	world->addRigidBody(plane->rigid_body);
	return plane;
}


void World::debugDraw() {
	world->debugDrawWorld();
}

// Box
// -----------------------------------------------------------------------------
btBoxShape* World::createBoxShape(float sizeX, float sizeY, float sizeZ) {
	btBoxShape* s = new btBoxShape(btVector3(sizeX, sizeY, sizeZ));
	return s;
}

rbb::Box* World::createBoxBody(btBoxShape* boxShape, const Vec3& position, const float mass) {
	rbb::Box* box = new rbb::Box();
	World::setupBody(box, boxShape, World::createTransform(position), mass);
	world->addRigidBody(box->rigid_body);
	return box;
}

// Util functions
// -----------------------------------------------------------------------------
// Creates a basic btTransform at position.
btTransform World::createTransform(const Vec3& position) {
	btTransform transform(btQuaternion(0.0f,0.0f,0.0f,1.0f), btVector3(position.x, position.y, position.z));
	return transform;
}

// Sets up a roxlu::body::Body type.
void World::setupBody(
	 rbb::Body* body
    ,btCollisionShape* collisionShape
	,const btTransform& transform
	,const float mass
)
{
	body->collision_shape = collisionShape;
	body->motion_state = new btDefaultMotionState(transform);
	btVector3 fall_inertia(0.0f, 0.0f, 0.f);
	btScalar shape_mass = btScalar(mass);
	body->collision_shape->calculateLocalInertia(shape_mass, fall_inertia);
	
	btRigidBody::btRigidBodyConstructionInfo body_ci(mass, body->motion_state, collisionShape, fall_inertia);
	body->rigid_body = new btRigidBody(body_ci);
	
	
}



}} // roxlu::bullet