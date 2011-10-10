#pragma once

#include "Camera.h"
#include "ArcBall.h"
#include "Vec2.h"
//#include "Types.h"

class EasyCam : public Camera {
public:
	EasyCam();
	//void enableEvents();
	//void disableEvents();
	inline void draw();
	void setup(float screenWidth, float screenHeight);
	void onMouseDown(float x, float y);
	void onMouseDragged(float x, float y);

	// EVENT HANDLERS
	// ------------------------------------
	/*
	void keyPressed(ofKeyEventArgs& rArgs);
	void keyReleased(ofKeyEventArgs& rArgs);
	void mousePressed(ofMouseEventArgs& rArgs);
	void mouseDragged(ofMouseEventArgs& rArgs);
	void mouseReleased(ofMouseEventArgs& rArgs);
	*/
	ArcBall arcball;
private:
	bool shift_down;
	Vec2 last_mouse_pos;
	
};

inline void EasyCam::draw() {
	arcball.draw();
}