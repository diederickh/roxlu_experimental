#include "EasyCam.h"
EasyCam::EasyCam():Camera()
	,shift_down(false)
{
	/*
	arcball.setCamera(*this);
	arcball.setRadius(std::min(
		(float)ofGetWidth() * 0.5
		,(float) ofGetHeight() * 0.5)
	);
	arcball.setCenter(ofGetWidth() * 0.5,ofGetHeight() * 0.5);
	translate(0,0,-12);
//	enableEvents();
*/	
	//printf("EASY CAM NEED TO IMPLEMENT THE ONMOUSEDRAGGED FUNCTION!");
	//printf("EASY CAM NEEDS TO IMPLEMENT FUNCTIOS TO PASS ON MOUSE EVENTS!");
}

void EasyCam::setup(float screenWidth, float screenHeight) {
	setScreenWidth(screenWidth);
	setScreenHeight(screenHeight);
	arcball.setCamera(*this);
	arcball.setRadius(std::min(
		(float)screenWidth * 0.5
		,(float)screenHeight * 0.5)
	);
	arcball.setCenter(screenWidth * 0.5,screenHeight * 0.5);
	translate(0,0,-12);
}

void EasyCam::onMouseDown(float x, float y) {
	arcball.onMouseDown(x, y);
}

void EasyCam::onMouseDragged(float x, float y) {
	arcball.onMouseDrag(x,y);
}


/*
void EasyCam::enableEvents() {
	ofAddListener(ofEvents.keyPressed,		this, &EasyCam::keyPressed);
	ofAddListener(ofEvents.keyReleased,		this, &EasyCam::keyReleased);
	ofAddListener(ofEvents.mousePressed,	this, &EasyCam::mousePressed);
	ofAddListener(ofEvents.mouseDragged,	this, &EasyCam::mouseDragged);
	ofAddListener(ofEvents.mouseReleased,	this, &EasyCam::mouseReleased);
}

void EasyCam::disableEvents() {
	ofRemoveListener(ofEvents.keyReleased,		this, &EasyCam::keyReleased);
	ofRemoveListener(ofEvents.keyPressed,		this, &EasyCam::keyPressed);
	ofRemoveListener(ofEvents.mousePressed,		this, &EasyCam::mousePressed);
	ofRemoveListener(ofEvents.mouseDragged,		this, &EasyCam::mouseDragged);
	ofRemoveListener(ofEvents.mouseReleased,	this, &EasyCam::mouseReleased);
}

void EasyCam::mouseReleased(ofMouseEventArgs& rArgs) {
}

void EasyCam::keyPressed(ofKeyEventArgs& rArgs) {
}

void EasyCam::keyReleased(ofKeyEventArgs& rArgs) {
}

void EasyCam::mousePressed(ofMouseEventArgs& rArgs) {
	if(rArgs.button == 0) {
		arcball.onMouseDown(rArgs.x, rArgs.y);
	}
	else {
		last_mouse_pos.x = rArgs.x;
		last_mouse_pos.y = rArgs.y;
	}
}

void EasyCam::mouseDragged(ofMouseEventArgs& rArgs) {
	if(rArgs.button == 0) {
		arcball.onMouseDrag(rArgs.x, rArgs.y);
	}
	else {
		float dx = (last_mouse_pos.x - rArgs.x);
		float dy = (last_mouse_pos.y - rArgs.y);
		float d = ((1.0f - InvSqrt(dx*dx+dy*dy))) * ((dy < 0) ? -1 : 1);
		position.z -= d * (fabs(position.z) *0.3);
		last_mouse_pos.x = rArgs.x;
		last_mouse_pos.y = rArgs.y;
	}
}

*/
