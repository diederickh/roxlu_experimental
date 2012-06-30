#include "Element.h"

namespace buttons {

Element::Element(int type, const string& name, int valueType) 
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
	,type(type)
	,name(name)
	,value_type(valueType)
{
	BSET_COLOR(col_bg_default, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(col_text, 0,0.48,0.98,0.9);
	bg_top_color = col_bg_default;
	bg_bottom_color = col_bg_default;
}

Element::~Element() {
}

} // namespace buttons