//#include "Element.h"
#include <buttons/Element.h>

namespace buttons {

Element::Element(int type, const string& name) 
	:x(0)
	,y(0)
	,w(0)
	,h(0)
	,is_visible(true)
	,is_child(false)
	,is_mouse_inside(false)
	,is_mouse_down_inside(false)
	,drag_inside(false)
	,needs_text_update(false)
	,needs_redraw(false)
	,state(BSTATE_NONE)
	,type(type)
	,name(name)
	,label("")
	,static_text(NULL)
	,dynamic_text(NULL)
{
	BSET_COLOR(col_bg_bottom_hover, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(col_bg_top_hover, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(col_bg_default, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(col_text, 0.9,0.9,0.9,0.9);
	bg_top_color = col_bg_default;
	bg_bottom_color = col_bg_default;
	setColor(col_bg_default[0], col_bg_default[1], col_bg_default[2]);
}

Element::~Element() {
}

} // namespace buttons