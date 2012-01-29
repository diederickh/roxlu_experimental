#include "Body.h"

namespace roxlu {
namespace bullet {
namespace body {

Body::Body() {
}

Body::~Body() {
}

void Body::copyMatrix(Mat4& mat) const {
	btTransform trans;
	motion_state->getWorldTransform(trans);
	trans.getOpenGLMatrix(mat.m);
}


}}} // roxlu::bullet::shape