#include "Shape.h"

namespace roxlu {


Shape::Shape() 
	:density(0.0f)
	,restitution(0.0f)
	,friction(0.0f)
	,body_type_set(false)
{
}

Shape::~Shape() {
}

} // roxlu