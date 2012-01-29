#ifndef ROXLU_EASYCAMH
#define ROXLU_EASYCAMH

#include "Camera.h"
#include "ArcBall.h"
#include "Vec2.h"
namespace roxlu {

class EasyCam : public Camera {
public:
	EasyCam();
	inline void debugDraw();
	void setup(float screenWidth, float screenHeight);
	void onMouseDown(float x, float y);
	void onMouseDragged(float x, float y);
	ArcBall arcball;
private:
	bool shift_down;
	Vec2 last_mouse_pos;
	
};

inline void EasyCam::debugDraw() {
	arcball.debugDraw();
}

} // roxlu
#endif