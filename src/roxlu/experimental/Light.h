#pragma once

#include "Vec3.h"
#include "Color.h"

namespace roxlu {

class Light {
public:
	Light();
	void setAmbientColor(float nR, float nG, float nB, float nA = 1.0f);
	void setDiffuseColor(float nR, float nG, float nB, float nA = 1.0f);
	void setSpecularColor(float nR, float nG, float nB, float nA = 1.0f);
	void setPosition(float nX, float nY, float nZ);
	void debugDraw();
	
	
	void disable();
	void enable(); 
	inline bool isEnabled() { return is_enabled; }
		
	bool is_enabled;
	
	Color4f ambient_col;
	Color4f diffuse_col;
	Color4f specular_col;
	Vec3 position;
};

}; // roxlu