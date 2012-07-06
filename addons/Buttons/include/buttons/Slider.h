#ifndef ROXLU_BSLIDERH
#define ROXLU_BSLIDERH

//#include "ofMain.h" // @todo remove
#include <buttons/Types.h>
#include <buttons/Element.h>

// #include "Types.h"
// #include "Element.h"
#include <sstream>

namespace buttons {

template<class T>
class Slider : public Element {
public:	

	enum ValueTypes {
		 SLIDER_INT
		,SLIDER_FLOAT
	};

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
		setColor(col_bg_default[0], col_bg_default[1], col_bg_default[2]);
		h = 22;
	}
	
	~Slider() {
	}
	
	Slider& setColor(const float* col, int num = 3) {
		if(num == 3) {
			setColor(col[0], col[1], col[2]);
		}
		else if (num == 4) {
			setColor(col[0], col[1], col[2], col[3]);
		}
		return *this;
	}
	
	Slider& setColor(const float r, const float g, const float b, const float a = 1.0) {
		Element::setColor(r,g,b,a);
		bar_empty_color[3] = 1.0f;
		bar_filled_color[3] = 1.0f;
		bar_filled_bottom[3] = 1.0f;
		
		// bar_empty_color: saturated default color
		float hue, sat, bright;
		Color::RGBToHLSf(r,g,b,&hue, &bright, &sat);
		Color::HLSToRGBf(hue, bright, sat * 0.5, &bar_empty_color[0], &bar_empty_color[1], &bar_empty_color[2]);

		// bar filled color 
		Color::HLSToRGBf(hue, bright * 1.7, sat * 1.7, &bar_filled_color[0], &bar_filled_color[1], &bar_filled_color[2]);
	
		// bar filled bottom color
		Color::HLSToRGBf(hue, bright * 0.9 , sat * 0.9, &bar_filled_bottom[0], &bar_filled_bottom[1], &bar_filled_bottom[2]);
		return *this;
	}

	void generateStaticText(Text& txt) {
		label_dx = txt.add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
	}
	
	
	void updateTextPosition(Text& staticText, Text& dynamicText) {
		staticText.setTextPosition(label_dx, x+4, y+2);
		updateDynamicText(dynamicText);
	}
	
	void generateDynamicText(Text& txt) {
		txtval_dx = txt.add(x,y+2, "0.0", col_text[0], col_text[1], col_text[2], col_text[3]);
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
		txt.updateText(txtval_dx, buf, col_text[0], col_text[1], col_text[2], col_text[3]);
	}
	
	
	void generateVertices(ButtonVertices& vd) {
		num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);

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
			bg_top_color = col_bg_default;
			bg_bottom_color = col_bg_default;
			needsRedraw();
		}
	}
	
	void onMouseEnter(int mx, int my) {
		bg_top_color = col_bg_top_hover;
		bg_bottom_color = col_bg_bottom_hover;
		needsRedraw();
	}
	
	void onMouseLeave(int mx, int my) {
		if(!drag_inside){
			bg_top_color = col_bg_default;
			bg_bottom_color = col_bg_default;
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
		p = (float)1.0f/nrange * (value-minv);
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