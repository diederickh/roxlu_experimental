#include "Application.h"
#include <cstdio>

Application::Application() {
}

Application::~Application() {
}

void Application::setup(float w, float h) {
	view_width = w;
	view_height = h;
	setClearColor(0.3, 0.3, 0.3, 1.0);
	
	v.generateTestPoints(300);
	cam.setup(w,h);
	cam.setNear(0.01);
	cam.translate(0,0,10);
}

void Application::update() {
}

void Application::draw() {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.pm().getPtr());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cam.vm().getPtr());
	
	v.veins.draw();
	v.veins.drawEdges();
	v.veins.drawCurrentBranch();
}
	
void Application::mouseDown(int x, int y) {
	cam.onMouseDown(x,view_height-y);
}

void Application::mouseUp(int x, int y) {
}

void Application::mouseMoved(int x, int y) {
}

void Application::mouseDragged(int x, int y, int dx, int dy) {
	cam.onMouseDragged(x,view_height-y);
}

void Application::rightMouseDown(int x, int y) {
}

void Application::rightMouseUp(int x, int y) {
}

void Application::rightMouseDragged(int x, int y, int dx, int dy) {
}

void Application::scrollWheel(float d) {
	cam.onScrollWheel(d * 0.1);
}

void Application::keyDown(unsigned char key) {
	if(key == ' ') {
		v.update();
	}
	else if(key == '1') {
		v.veins.nextBranch();
	}
	else if(key == '2') {
		v.veins.findBranches();
	}
}

void Application::keyUp(unsigned char key) {
}

void Application::resized(float w, float h) {
}
 