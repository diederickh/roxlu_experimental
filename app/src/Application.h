#ifndef ROXLU_APPLICATION_INTERFACE
#define ROXLU_APPLICATION_INTERFACE

#include "ApplicationBase.h"
#include "Roxlu.h"
#include "Veins3D.h"

class Application : public ApplicationBase {
public:
	Application();
	~Application();
	
	void setup(float w, float h);
	void update();
	void draw();
	
	void mouseDown(int x, int y);
	void mouseUp(int x, int y);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int dx, int dy);
	void scrollWheel(float d);
	
	void rightMouseDown(int x, int y);
	void rightMouseDragged(int x, int y, int dx, int dy);
	void rightMouseUp(int x, int y);
	
	void keyDown(unsigned char key);
	void keyUp(unsigned char key);
	
	void resized(float w, float h);
		

	EasyCam cam;
	Veins3D v;
};



#endif