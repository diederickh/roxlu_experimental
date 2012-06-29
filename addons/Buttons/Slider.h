#ifndef ROXLU_BSLIDERH
#define ROXLU_BSLIDERH

#include "ofMain.h"
#include "Types.h"
#include "Element.h"
#include <sstream>

namespace buttons {

template<class T>
class Slider : public Element {
public:	

	enum ValueTypes {
		 SLIDER_INT
		,SLIDER_FLOAT
	};
	/*
	
Slider::Slider(float& val, const string& name)
	:value(val)
	,Element(BTYPE_SLIDER, name)
	,minv(1)
	,maxv(1000)
	,p(0.5)
	,txtval_dx(-1)
	,label_dx(-1)
{
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	h = 22;
}

	*/
	Slider(T& val, const string& name, int valueType) 
		:value(val)
		,Element(BTYPE_SLIDER, name, BVALUE_FLOAT)
		,minv(1)
		,maxv(1000)
		,p(0.0)
		,txtval_dx(-1)
		,label_dx(-1)
		,value_type(valueType)

	{
		BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		h = 22;
	}
	
	~Slider() {
	}

	void generateStaticText(Text& txt) {
		label_dx = txt.add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
	}
	
	
	void updateTextPosition(Text& staticText, Text& dynamicText) {
		staticText.setTextPosition(label_dx, x+4, y+2);
		updateDynamicText(dynamicText);
	}
	
	void generateDynamicText(Text& txt) {
		txtval_dx = txt.add(x,y+2, "0.0",0,0.48,0.98,0.9);
		txt.setTextAlign(txtval_dx, TEXT_ALIGN_RIGHT, x+(w - 5));
	}
	
	void updateDynamicText(Text& txt) {
		txt.setTextPosition(txtval_dx, x, y+2);
		char buf[256];
		float v;
		std::stringstream ss;
		ss << value;
		ss >> v;
		sprintf(buf, "%5.4f", v);
		txt.updateText(txtval_dx, buf,0,0.48,0.98,0.9);
	}
	
	
	void generateVertices(ButtonVertices& vd) {
		num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
		BSET_COLOR(bar_filled_color, 0.0,0.8,0.4,1.0);
		BSET_COLOR(bar_filled_bottom, 0.0,0.4,0.2,1.0);
		BSET_COLOR(bar_empty_color, 0.0,0.8,0.4,0.1);

		p = std::min<float>(1.0, p);
		
		int bar_h = 4; // bar height
		int bar_filled_w = w * p;
		int bar_empty_w = w * (1.0 - p);
		int bar_x = x; // start x
		int bar_y = (y + h) - bar_h; // start y

		num_vertices += buttons::createRect(vd, bar_x, bar_y, bar_filled_w, bar_h, bar_filled_color, bar_filled_bottom);
		num_vertices += buttons::createRect(vd, bar_x + bar_filled_w, bar_y, bar_empty_w, bar_h, bar_empty_color, bar_empty_color);
	}

	void onMouseUp(int mx, int my) {
		if(!is_mouse_inside) { // mouse up after
			BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
			BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
			needsRedraw();
		}
	}
	
	void onMouseEnter(int mx, int my) {
		BSET_COLOR(bg_top_color, 0.0, 0.41, 0.39, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		needsRedraw();
	}
	
	void onMouseLeave(int mx, int my) {
		if(!drag_inside){
			BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
			BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
			needsRedraw();
		}
	}
	
	void onMouseDragged(int mx, int my, int dx, int dy) {
		if(drag_inside) {
			int local_x = std::min<int>(x+w,std::max<int>(x,mx));
			p = (float)(local_x-x)/w;
			value = minv + (p * (maxv-minv));
			needsRedraw();
			needsTextUpdate();
		}
	}
	void setPercentage(const float perc) {
		p = perc;
		value = minv +((maxv-minv) * p);
	}

	void setValue(const T& v) {
		value = v;
		T absv = std::abs(minv + std::abs(v));
		T nrange = maxv-minv;
		p = (float)absv/nrange;
	}
	
	Slider& setMin(const T& m) {
		minv = m; 
		return *this;
	}
	
	Slider& setMax(const T& m) {
		maxv = m;
		return *this;
	}

	T& value;
	T minv; // min value
	T maxv; // max value 
	float p; // current percentage
	int value_type; // SLIDER_FLOAT, SLIDER_INT (for storage)
	
	float bg_top_color[4];
	float bg_bottom_color[4];
	float bar_filled_color[4];
	float bar_filled_bottom[4];
	float bar_empty_color[4];
	int txtval_dx;
	int label_dx;
};

typedef Slider<float> Sliderf;
typedef Slider<int> Slideri;

//inline Slider& Slider::setMin(const float& m) {
//	minv = m;
//	return *this;
//}
//
//inline Slider& Slider::setMax(const float& m) {
//	maxv = m;
//	return *this;
//}

} // namespace buttons

#endif