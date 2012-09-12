#include <box2dwrapper/Shape.h>

namespace roxlu {
namespace box2d { 

Shape::Shape() 
	:density(0.0f)
	,restitution(0.0f)
	,friction(0.0f)
	,body_type_set(false)
{
}

Shape::~Shape() {
}

}} // roxlu::box2d
