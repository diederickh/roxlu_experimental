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
	,value_changed(false)
	,needs_redraw(false)
	,state(BSTATE_NONE)
	,type(type)
	,name(name)
	,label("")
	,static_text(NULL)
	,dynamic_text(NULL)
	,col_sat(0.2)
	,col_bright(0.27)
	,col_hue(0.5)
	,event_data(NULL)
	,parent(NULL)
{

	setColor(0.13);
	bg_top_color = col_bg_default;
	bg_bottom_color = col_bg_default;
}

Element::~Element() {
}

} // namespace buttonsprint n
