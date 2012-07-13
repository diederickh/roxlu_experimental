#include <buttons/Spline.h>

namespace buttons {

Spline::Spline() {
}

Spline::~Spline() {
}

void Spline::createCurves(int numCurves) {
	points.clear();
	int num = 1 + numCurves * 3; // 4 points for a cubic bezier
	
	for(int i = 0; i < num; ++i) {
		float p = float(i)/(num-1);
		points.push_back(CurvePoint(p,0.5));
	}
}

void Spline::add(const float x, const float y) {
	points.push_back(CurvePoint(x, y));
}

}