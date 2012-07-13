#include <buttons/Spline.h>

namespace buttons {

Spline::Spline() {
}

Spline::~Spline() {
}

void Spline::createCurves(int numCurves) {
	points.clear();
	int num = numCurves * 4; // 4 points for a cubic bezier
	for(int i = 0; i < num; ++i) {
		float p = float(i)/(num-1);
		points.push_back(CurvePoint(p,0.5));
	}
}

}