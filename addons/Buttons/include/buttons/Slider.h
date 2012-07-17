#ifndef ROXLU_BSLIDERH
#define ROXLU_BSLIDERH

#include <buttons/Types.h>
#include <buttons/Element.h>
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
		,Element(BTYPE_SLIDER, name)
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
	
	Slider& setColor(const float hue, const float sat, const float bright, const float a = 1.0) {
		Element::setColor(hue,sat,bright,a);
		bar_empty_color[3] = a;
		bar_filled_color[3] = a;
		bar_filled_bottom[3] = a;
		
		HSL_to_RGB(hue, sat, bright - 0.2, &bar_empty_color[0], &bar_empty_color[1], &bar_empty_color[2]);
		HSL_to_RGB(hue, sat, bright + 0.2, &bar_filled_color[0], &bar_filled_color[1], &bar_filled_color[2]);
		HSL_to_RGB(hue, sat, bright - 0.1, &bar_filled_bottom[0], &bar_filled_bottom[1], &bar_filled_bottom[2]);
		return *this;
	}

	void generateStaticText() {
		label_dx = static_text->add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
	}
		
	void updateTextPosition() {
		static_text->setTextPosition(label_dx, x+4, y+2);
		updateDynamicText();
	}
	
	void generateDynamicText() {
		txtval_dx = dynamic_text->add(x,y+2, "0.0", col_text[0], col_text[1], col_text[2], col_text[3]);
		dynamic_text->setTextAlign(txtval_dx, TEXT_ALIGN_RIGHT, (w - 5));
	}
	
	void updateDynamicText() {
		char buf[256];
		valueToChar(buf, value);
		dynamic_text->updateText(txtval_dx, buf, col_text[0], col_text[1], col_text[2], col_text[3]);
		dynamic_text->setTextPosition(txtval_dx, x, y+2);		
	}
	
	void valueToChar(char* buf, int v) {
		sprintf(buf, "%d", v);
	}
	
	void valueToChar(char* buf, float v) {
		sprintf(buf, "%5.4f", v);
	}
		
	void generateVertices(ButtonVertices& vd) {
		buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);

		p = std::min<float>(1.0, p);
		
		int bar_h = 4; // bar height
		int bar_filled_w = w * p;
		int bar_empty_w = w * (1.0 - p);
		int bar_x = x; // start x
		int bar_y = (y + h) - bar_h; // start y

		buttons::createRect(vd, bar_x, bar_y, bar_filled_w, bar_h, bar_filled_color, bar_filled_bottom);
		buttons::createRect(vd, bar_x + bar_filled_w, bar_y, bar_empty_w, bar_h, bar_empty_color, bar_empty_color);
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
	
	bool canSave() {
		return true;
	}
	
	void save(std::ofstream& ofs) {
		size_t data_size = sizeof(int) + sizeof(T);
		ofs.write((char*)&data_size, sizeof(size_t));

		ofs.write((char*)&value_type, sizeof(int));
		ofs.write((char*)&value, sizeof(T));
	}
	
	void load(std::ifstream& ifs) {
		ifs.read((char*)&value_type, sizeof(int));
		ifs.read((char*)&value, sizeof(T));
		setValue(value);
		needsRedraw();
		needsTextUpdate();
	}
	
	
	void hide() {
		this->is_visible = false;
		static_text->setTextVisible(label_dx, false);
		dynamic_text->setTextVisible(txtval_dx, false);
	}
	
	void show() {
		this->is_visible = true;
		static_text->setTextVisible(label_dx, true);
		dynamic_text->setTextVisible(txtval_dx, true);
		updateTextPosition();
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

} // namespace buttons

#endif