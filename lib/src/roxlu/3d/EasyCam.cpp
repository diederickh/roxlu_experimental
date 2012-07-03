// #include "EasyCam.h"

#include <roxlu/3d/EasyCam.h>

namespace roxlu {

EasyCam::EasyCam():Camera()
	,shift_down(false)
{
}

void EasyCam::setup(float screenWidth, float screenHeight) {
	setScreenWidth(screenWidth);
	setScreenHeight(screenHeight);
	arcball.setCamera(*this);
	arcball.setScreenSize(screenWidth, screenHeight);

	translate(0,0,5);
}

void EasyCam::onScrollWheel(float d) {
	translate(0,0,d);
}

void EasyCam::onMouseDown(float x, float y) {
	last_x = x;
	last_y = y;

	arcball.onMouseDown(x, y);
}

void EasyCam::onMouseDragged(float x, float y, bool move) {	
	if(move) {
		float dx = x - last_x;
		float dy = y - last_y;
		float max_x = (screen_width * 0.25);
		float max_y = (screen_height * 0.25);
		float px = ((dx) / max_x) * MAX(0.4 * position.z, 0.2);
		float py = ((dy) / max_y) * MAX(0.4 * position.z, 0.2);
		translate(-px, py, 0);

//		printf("dx: %f, dy: %f, px: %f, py: %f, %f\n", dx, dy, px, py, position.z);
	
	}
	else {
		arcball.onMouseDrag(x,y);
	}
	last_x = x;
	last_y = y;

}

} // roxlu
