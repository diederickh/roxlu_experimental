#include "TrackpadCam.h"
#include "Trackpad.h"

namespace roxlu {

TrackpadCam* TrackpadCam::instance = NULL;

// trackpad interaction.
// -----------------------------------------------------------------------------
void zoom_callback(float z) {
	EasyCam* cam = TrackpadCam::instance->renderer->getCameraPtr();
	Vec3 dir = cam->getPosition();
	dir.normalize();
	dir *= (z * 10.0f);
	cam->translate(dir.x, dir.y, dir.z);
}

void rotate_callback(float degree) {
	TrackpadCam::instance->renderer->getCameraPtr()->rotate((degree * 3.5) * DEG_TO_RAD,0,1,0);
}

void scroll_callback(float delta_x, float delta_y) {
	TrackpadCam::instance->renderer->getCameraPtr()->translate(delta_x * 0.1,0, delta_y * 0.1);
}

void left_mouse_down_callback(float x, float y) {
	TrackpadCam::instance->renderer->getCameraPtr()->onMouseDown(x,y);
	TrackpadCam::instance->mouse_down = true;
}

void left_mouse_up_callback(float x, float y) {	
	TrackpadCam::instance->mouse_down = false;
}

void mouse_dragged_callback(float x, float y, float dx, float dy) {
	TrackpadCam::instance->renderer->getCameraPtr()->onMouseDragged(x,y);
}

void mouse_move_callback(float x, float y) {
	if(TrackpadCam::instance->mouse_down) {
		TrackpadCam::instance->renderer->getCameraPtr()->onMouseDragged(x,y);	
	}
}

// Trackpad setup.
// -----------------------------------------------------------------------------
TrackpadCam::TrackpadCam() 
	:mouse_down(false)
{
}

void TrackpadCam::setup(Renderer& theRenderer) {
	renderer = &theRenderer;
	trackpad_init();
	trackpad_set_mouse_scroll_callback(scroll_callback);
	trackpad_set_magnify_callback(zoom_callback);
	trackpad_set_rotate_callback(rotate_callback);
	trackpad_set_mouse_move_callback(mouse_move_callback);
	trackpad_set_left_mouse_down_callback(left_mouse_down_callback);
	trackpad_set_left_mouse_up_callback(left_mouse_up_callback);
	trackpad_set_left_mouse_dragged_callback(mouse_dragged_callback);
	TrackpadCam::instance = this;
}

};