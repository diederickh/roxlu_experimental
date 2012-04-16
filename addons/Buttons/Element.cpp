#include "Element.h"

namespace buttons {

Element::Element(int type) 
	:x(0)
	,y(0)
	,w(0)
	,h(0)
	,is_mouse_inside(false)
	,is_mouse_down_inside(false)
	,drag_inside(false)
	,needs_text_update(false)
	,needs_redraw(false)
	,state(BSTATE_NONE)
	,start_dx(0)
	,end_dx(0)
	,type(type)
{
}

Element::~Element() {
}

} // namespace buttons