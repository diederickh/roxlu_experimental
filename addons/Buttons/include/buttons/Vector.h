#ifndef ROXLU_VECTORH
#define ROXLU_VECTORH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

template<class T>
class Vector : public Element {
public:	
	Vector(T* value, const string& name);
	~Vector();

	void generateStaticText();
	void updateTextPosition();
	void generateVertices(ButtonVertices& shapeVertices);

	void onMouseMoved(int mx, int my);
	void onMouseDragged(int mx, int my, int dx, int dy);	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);
	
	void save(std::ofstream& ofs);
	void load(std::ifstream& ifs);
	bool canSave();
	
	Vector<T>& setColor(const float hue, float a = 1.0);
	/*
	Vector<T>& setX(const float min, const float max); // set min/max x
	Vector<T>& setY(const float min, const float max); // set min/max y
	*/
	void hide();
	void show();
	
private:
	void calculateCoordinates();

	float circle_bg_col[4];
	float rect_bg_top_col[4];
	float rect_bg_bottom_col[4];
	float radius; 
	float cx;
	float cy;
	float angle;
	bool is_rotating;
	float test_col[4];
	/*
	float calculatePercentage(float value, float min, float maxv);

	bool near_tl;
	bool near_br;
	bool inside_rect;
	*/

public:
	
  	int label_dx;
	T* value;

	// Used for drawing
	float bg_x;
	float bg_y; 
	float bg_h;
	float bg_w; 
};

	template<class T>
	Vector<T>::Vector(T* value, const string& name)
		:Element(BTYPE_RECTANGLE, name)
		,label_dx(0)
		,bg_x(0)
		,bg_y(0)
		,bg_h(0)
		,bg_w(0)
		,value(value)
		,radius(10)
		,cx(0)
		,cy(0)
		,angle(0)
		,is_rotating(false)
	{
		this->h = 120;
	}

	template<class T>
	Vector<T>::~Vector() {
	}

	template<class T>
	void Vector<T>::generateStaticText() {
		label_dx = static_text->add(x+4, y+5, label, 0.9, 0.9, 0.9, 0.9);
	}

	template<class T>
	void Vector<T>::updateTextPosition() {
		static_text->setTextPosition(label_dx, this->x+4, this->y+5);
	}

	template<class T>
	void Vector<T>::generateVertices(ButtonVertices& vd) {
		calculateCoordinates();

		// Background + pad area
		buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color); // background
		buttons::createRect(vd, bg_x, bg_y, bg_w, bg_h, this->rect_bg_top_col, this->rect_bg_bottom_col); // positioning area
		buttons::createCircle(vd, cx, cy, radius, circle_bg_col);

		// Arrow
		float end_x = cx + cos(angle) * (radius * 0.8);
		float end_y = cy + sin(angle) * (radius * 0.8);
		
		float dir_x = end_x - cx;
		float dir_y = end_y - cy;
		float dx = (dir_x * dir_x);
		float dy = (dir_y * dir_y);
		float len = sqrtf(dx + dy);

		dir_y /= len;
		dir_x /= len;
				printf("l: %f\n", len);
		float perp_x = -dir_y;
		float perp_y = dir_x;
		
		float point_w = 9.0f;
		float arrow_w = 2.5f;
		float a[2] = {cx - (perp_x * arrow_w), cy - (perp_y * arrow_w)};
		float b[2] = {cx + (perp_x * arrow_w), cy + (perp_y * arrow_w)};
		float c[2] = {end_x - (perp_x * arrow_w), end_y - (perp_y * arrow_w)};
		float d[2] = {end_x + (perp_x * arrow_w), end_y + (perp_y * arrow_w)};
		float e[2] = {end_x - (perp_x * point_w), end_y - (perp_y * point_w)};
		float f[2] = {end_x + (perp_x * point_w), end_y + (perp_y * point_w)};
		float g[2] = {cx + (dir_x * len * 1.4), cy + (dir_y * len * 1.4)};
		buttons::createCircle(vd, end_x, end_y, 3.0f, circle_bg_col);
		buttons::createCircle(vd, a[0], a[1], 3.0f, test_col);
		buttons::createCircle(vd, b[0], b[1], 3.0f, test_col);
		buttons::createCircle(vd, c[0], c[1], 3.0f, test_col);
		buttons::createCircle(vd, d[0], d[1], 3.0f, test_col);
		buttons::createCircle(vd, e[0], e[1], 3.0f, test_col);
		buttons::createCircle(vd, f[0], f[1], 3.0f, test_col);
		buttons::createCircle(vd, g[0], g[1], 3.0f, test_col);
	}

	template<class T>
	void Vector<T>::onMouseMoved(int mx, int my) {
		if(drag_inside && !is_rotating) {
			is_rotating = true;
		}
		if(is_rotating) {
			// calculate angle
			float dy = (my - cy);
			float dx = (mx - cx);
			float atan_angle = atan2(dy, dx);
			if(atan_angle < 0) {
				angle = atan_angle + TWO_PI;
			}
			else {
				angle = atan_angle;
			}
			needsRedraw();
			//printf("is rotating: %f.\n", angle * (180.0f/3.14));
		}
	}

	template<class T>
	void Vector<T>::onMouseDragged(int mx, int my, int dx, int dy) {

	}

	template<class T>
	void Vector<T>::calculateCoordinates() {
		bg_w = this->w - 10;
		bg_h = this->h - 25;
		bg_x = this->x + (this->w - bg_w) / 2;
		bg_y = this->y + 20;
		radius = (bg_h * 0.5);
		cx = bg_x + (bg_w * 0.5f);
		cy = bg_y + (bg_h * 0.5f);
	}

	/*
	template<class T>
	float Vector<T>::calculatePercentage(float value, float minv, float maxv) {
		float p = 1.0f/(maxv-minv) * value - (minv/(maxv-minv)); 
		p = BLIMIT_FLOAT(p, 0.0f, 1.0f);
		return p;
	}
	*/

	template<class T>
	void Vector<T>::onMouseDown(int mx, int my) {
	}

	template<class T>
	void Vector<T>::onMouseUp(int mx, int my) {
		is_rotating = false;
	}

	template<class T>
	void Vector<T>::onMouseEnter(int mx, int my) {
	}

	template<class T>
	void Vector<T>::onMouseLeave(int mx, int my) {
	}

	template<class T>
	void Vector<T>::onMouseClick(int mx, int my) {
	}

	template<class T>
	void Vector<T>::save(std::ofstream& ofs) {
		/*
		size_t data_size = sizeof(T) * 4; // 4 items (top left x/y and bottom right x/y
		ofs.write((char*)&data_size, sizeof(size_t)); // necessary !! (used to skip data when we remove an element)
		ofs.write((char*)value, sizeof(T) * 4);
		*/
	}

	template<class T>
	void Vector<T>::load(std::ifstream& ifs) {
		/*
		ifs.read((char*)value, sizeof(T) * 4);
		perc_top_left[0] = calculatePercentage(*(value), min_x_value, max_x_value);
		perc_top_left[1] = calculatePercentage(*(value+1), min_y_value, max_y_value);
		perc_bottom_right[0] = calculatePercentage(*(value+2), min_x_value, max_x_value);
		perc_bottom_right[1] = calculatePercentage(*(value+3), min_y_value, max_y_value);
		needsRedraw();
		*/
	}

	template<class T>
	bool Vector<T>::canSave() {
		return false;
	}

	template<class T>
	Vector<T>& Vector<T>::setColor(const float hue, float a) {
		Element::setColor(hue, a);

		rect_bg_top_col[3] = a;
		rect_bg_bottom_col[3] = a;
		HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, rect_bg_top_col, rect_bg_top_col+1, rect_bg_top_col+2);
		HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, rect_bg_bottom_col, rect_bg_bottom_col+1, rect_bg_bottom_col+2);
		BSET_COLOR(circle_bg_col, 1.0f, 1.0f, 1.0f, 0.05);
		BSET_COLOR(test_col, 1.0f, 0.0f, 0.0f ,1.0f);
		/*
		HSL_to_RGB(col_hue, col_sat, col_bright + 0.2, rect_line_col, rect_line_col+1, rect_line_col+2);
		BSET_COLOR(rect_line_col, 1.0f, 1.0f, 1.0f, 0.3f);

		BSET_COLOR(rect_handle_active_col, 1.0f, 1.0f, 1.0f, 1.0f);
		BSET_COLOR(rect_handle_inactive_col, 1.0f, 1.0f, 1.0f, 0.5f);
		BSET_COLOR(rect_drag_col, 1.0f, 1.0f, 1.0f, 0.05f);
		*/
		return *this;
	}

	template<class T>
	void Vector<T>::hide() {
		this->is_visible = false;
		this->static_text->setTextVisible(label_dx, false);
	}

	template<class T>
	void Vector<T>::show() {
		this->is_visible = true;
		this->static_text->setTextVisible(label_dx, true);
	}
} // namespace buttons

#endif
