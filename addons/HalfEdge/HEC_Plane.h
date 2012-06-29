#ifndef HEC_PLANEH
#define HEC_PLANEH

#include "HEC_Creator.h"

namespace roxlu {

class HE_Mesh;

class HEC_Plane : public HEC_Creator {
public:
	HEC_Plane();
	HEC_Plane& setNumCols(int nc);
	HEC_Plane& setNumRows(int nr);
	HEC_Plane& setWidth(float w);
	HEC_Plane& setHeight(float h);
	bool create(HE_Mesh& mesh);	
	
private:
	float width;
	float height;
	int num_cols;
	int num_rows;
};

inline HEC_Plane& HEC_Plane::setNumCols(int nc) {
	num_cols = nc;
	return *this;
}

inline HEC_Plane& HEC_Plane::setNumRows(int nr) {
	num_rows = nr;
	return *this;
}

inline HEC_Plane& HEC_Plane::setWidth(float w) {
	width = w;
	return *this;
}

inline HEC_Plane& HEC_Plane::setHeight(float h) {
	height = h;
	return *this;
}

}; // roxlu
#endif