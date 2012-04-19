#include "Shape.h"

namespace roxlu {

//float Shape::pixels_per_meter = 100.0f;
//float Shape::meter_per_pixels = 0.01f;

Shape::Shape() 
	:density(0.0f)
	,restitution(0.0f)
	,friction(0.0f)
{
}

Shape::~Shape() {
}

} // roxlu