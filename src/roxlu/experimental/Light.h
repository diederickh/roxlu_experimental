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
	inline Vec3& getPosition();
	inline Color4& getDiffuseColor();
	
	void disable();
	void enable(); 
	inline bool isEnabled() { return is_enabled; }
		
	bool is_enabled;
	
	Color4 ambient_col;
	Color4 diffuse_col;
	Color4 specular_col;
	Vec3 position;
};

inline Vec3& Light::getPosition() {
	return position;
}

inline Color4& Light::getDiffuseColor() {
	return diffuse_col;
}


}; // roxlu