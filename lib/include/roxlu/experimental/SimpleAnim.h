#ifndef ROXLU_SIMPLEANIMH
#define ROXLU_SIMPLEANIMH

#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>

#include <roxlu/experimental/Timer.h>

using std::string;
using std::map;
using std::vector;

/**
 
 Plays back simple animations from Blender; interpolates
 between positions based on the set FPS.
 
 
 Use this blender script to export the animations of the selected objects:
 -------------------------------------------------------------------------
 
	import bpy

	# export the animation frame by frame for selected object
	ofs = open("/Users/diederickhuijbers/Documents/programming/c++/of/TripTych/apps/diederick/006_intell_2d_cloth_shaper/bin/data/hand.anim", "w")
	start = 0
	stop = 73
	selected_objects = bpy.context.selected_objects

	bpy.context.scene.frame_current = start
	for obj in selected_objects:
		bpy.context.scene.frame_set(start)
		data = "o %s\n" % obj.name
		ofs.write(data)
		for i in range(start, stop):
			pos =  obj.location
			bpy.context.scene.frame_set(i)
			data = "p %f %f %f\n" % (pos.y, pos.z, pos.x)
			ofs.write(data)
		
	bpy.context.scene.frame_current = start
	ofs.close()

 
 */
 
namespace roxlu {
	
enum SimpleAnimStates {
	ANIM_NONE
	,ANIM_PLAY
	,ANIM_PAUSE
	,ANIM_STOP
};

class SimpleAnim {
public:	
	struct AnimData {
		float pos[3];
		void print() {	
			printf("x: %f, y: %f, z: %f\n", pos[0], pos[1], pos[2]);
		}
		
		void setPos(const float* p) {
			pos[0] = p[0];
			pos[1] = p[1];
			pos[2] = p[2];
		}
		
	};
		
	SimpleAnim();	
	void addFrame(const string& group, const float* pos);	
	bool load(const string& filepath);
	bool save(const string& filepath);
	void setFPS(const unsigned int& fps);
	void play();
	void update();
	void getPosition(const string& group, float* dest);
	bool hasGroup(const string& group);
	uint64_t now();
	
	int64_t millis_per_frame;
	uint64_t last_step;
	uint64_t next_frame;
	float perc;
	int total_frames;
	int mode;
	int frame;
	map<string, vector<AnimData> > animations;
};


inline uint64_t SimpleAnim::now() {
	return Timer::now();
	/*
	timeval time;
	gettimeofday(&time, NULL);
	uint64_t n = time.tv_usec;
	n /= 1000; // convert seconds to millis
	n += (time.tv_sec * 1000); // convert micros to millis
	return n;
	*/
}

} // roxlu
#endif