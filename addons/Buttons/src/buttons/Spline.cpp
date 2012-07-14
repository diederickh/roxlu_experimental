#include <buttons/Spline.h>

namespace buttons {

Spline::Spline() {
}

Spline::~Spline() {
}

void Spline::createPoints(int numPoints) {
	points.clear();
	//int num = 1 + numCurves * 3; // 4 points for a cubic spline
	if(numPoints < 4) {
		numPoints = 4;
	}
	for(int i = 0; i < numPoints; ++i) {
		float p = float(i)/(numPoints-1);
		points.push_back(CurvePoint(p,0.5));
	}
}

void Spline::add(const float x, const float y) {
	points.push_back(CurvePoint(x, y));
}

}