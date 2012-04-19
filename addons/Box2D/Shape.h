#ifndef ROXLU_BOX2D_SHAPEH
#define ROXLU_BOX2D_SHAPEH


#include <Box2D/Box2D.h>
#include "Utils.h"
#include "Vec2.h"
using roxlu::Vec2;

namespace roxlu {

class Shape {
public:
	Shape();
	~Shape();
	
	virtual void make(b2World& w) = 0;
	virtual void setRestitution(const float& v);
	virtual void setDensity(const float& v);
	virtual void setFriction(const float& v);
	virtual void setPosition(const float& x, const float& y);
	virtual void setAllowSleep(const bool& allow);
	virtual void setAwake(const bool& awake);
	virtual void setDamping(const float& v);
	
	virtual void addForce(const float& x, const float& y);
	Vec2 getScreenPosition();
	
	b2Body*			body;
	b2BodyDef		body_def;
	b2FixtureDef	fixture_def;

	float density;
	float restitution;
	float friction;
	
//	static float pixels_per_meter;
//	static float meter_per_pixels;
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

inline void Shape::setPosition(const float& x, const float& y) {
	body_def.position.Set(PIXELS_TO_METERS(x),PIXELS_TO_METERS(y));
}

inline void Shape::addForce(const float& x, const float& y) {
	//printf("ff: %f\n", body->GetPosition().x);
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

inline Vec2 Shape::getScreenPosition() {
	b2Vec2 p = body->GetPosition();
	return Vec2(METERS_TO_PIXELS(p.x), METERS_TO_PIXELS(p.y));
}

} // namespace roxlu

#endif