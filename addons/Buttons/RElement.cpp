#include "RElement.h"

RElement::RElement() 
	:x(0)
	,y(0)
	,w(0)
	,h(0)
	,is_mouse_inside(false)
	,is_mouse_down_inside(false)
	,is_changed(false)
	,state(RSTATE_NONE)
	,start_dx(0)
	,end_dx(0)
{
}

RElement::~RElement() {
}