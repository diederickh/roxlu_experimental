#include "EasyCam.h"
namespace roxlu {

EasyCam::EasyCam():Camera()
	,shift_down(false)
{
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
	translate(0,0,-5);
}

void EasyCam::onMouseDown(float x, float y) {
	arcball.onMouseDown(x, y);
}

void EasyCam::onMouseDragged(float x, float y) {
	arcball.onMouseDrag(x,y);
}

} // roxlu
