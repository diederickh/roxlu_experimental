#ifndef ROXLU_BOX2D_SHAPEH
#define ROXLU_BOX2D_SHAPEH


#include <Box2D/Box2D.h>
#include <box2dwrapper/Utils.h>
#include <roxlu/math/Vec2.h>

using roxlu::Vec2;

namespace roxlu {
namespace box2d {

class Shape {
public:
	Shape();
	~Shape();
	
	virtual void make(b2World& w) = 0;
	virtual void setRestitution(const float& v);
	virtual void setDensity(const float& v);
	virtual void setFriction(const float& v);
	virtual void setTransform(const float& x, const float& y, const float& angle);
	virtual void setPosition(const float& x, const float& y);
	virtual void setAllowSleep(const bool& allow);
	virtual void setAwake(const bool& awake);
	virtual void setFixedRotation(const bool fixed);
	virtual void setDamping(const float& v);
	virtual void setBodyType(b2BodyType type);
	virtual void addForce(const float& x, const float& y);
	
	Vec2 getPosition();
	
	b2Body*			body;
	b2BodyDef		body_def;
	b2FixtureDef	fixture_def;

	float density;
	float restitution;
	float friction;
	
	bool body_type_set;
	
};


inline void Shape::setRestitution(const float& v) {
	restitution = v;
}

inline void Shape::setDensity(const float& v) {
	density = v;
}

inline void Shape::setFriction(const float& v) {
	friction = v;
}

inline void Shape::setTransform(const float& x, const float& y, const float& angle) {
	body->SetTransform(b2Vec2(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y)), angle);
}

inline void Shape::setPosition(const float& x, const float& y) {
	body_def.position.Set(PIXELS_TO_METERS(x),PIXELS_TO_METERS(y));
}

inline void Shape::addForce(const float& x, const float& y) {
	body->ApplyForce(b2Vec2(x,y), body->GetPosition());
}

inline void Shape::setAllowSleep(const bool& allow) {
	body_def.allowSleep = allow;
}

inline void Shape::setAwake(const bool& awake) {
	body_def.awake = awake;
}

// use something between 0.1 and 0.0
inline void Shape::setDamping(const float& v) {
	body_def.linearDamping = v;
}

inline void Shape::setFixedRotation(bool f) {
	body_def.fixedRotation = f;
}

inline Vec2 Shape::getPosition() {
	b2Vec2 p = body->GetPosition();
	return Vec2(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y));
}

// b2_kinematicBody, b2_staticBody, b2_dynamicBody 
 inline void Shape::setBodyType(b2BodyType t) {
	body_def.type = t;
	body_type_set = true;
}

}} // namespace roxlu::box2d

#endif
