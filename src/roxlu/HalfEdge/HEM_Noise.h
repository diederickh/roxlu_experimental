#ifndef HEM_NOISEH
#define HEM_NOISEH

#include "HEM_Modifier.h"

namespace roxlu {

class HE_Mesh;
class HEM_Noise : public HEM_Modifier {
public:
	HEM_Noise();
	bool modify(HE_Mesh& m);
	
	inline HEM_Noise& setScaleX(float xs);
	inline HEM_Noise& setScaleY(float ys);
	inline HEM_Noise& setScaleZ(float zs);
	inline HEM_Noise& setDisplacement(float disp);
	
	// noise scales.
	float x_scale;
	float y_scale;
	float z_scale;
	float displacement; 
};

inline HEM_Noise& HEM_Noise::setScaleX(float xs) {
	x_scale = xs;
	return *this;
}

inline HEM_Noise& HEM_Noise::setScaleY(float ys) {
	y_scale = ys;
	return *this;
}

inline HEM_Noise& HEM_Noise::setScaleZ(float zs) {
	z_scale = zs;
	return *this;
}

inline HEM_Noise& HEM_Noise::setDisplacement(float disp) {
	displacement = disp;
}

}; // roxlu

#endif