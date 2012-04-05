#ifndef ROXLU_TRACKPADCAMH
#define ROXLU_TRACKPADCAMH

#include "Roxlu.h"

// Apply trackpack implementation which repositions the camera based on 
// TrackpadCam gestures.
namespace roxlu {
class TrackpadCam {
public:
	TrackpadCam();
	void setup(Renderer& renderer);
	
	bool mouse_down;
	Renderer* renderer;
	static TrackpadCam* instance;
};

};

#endif