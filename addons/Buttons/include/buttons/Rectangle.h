#ifndef ROXLU_RECTANGLEH
#define ROXLU_RECTANGLEH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

template<class T>
class Rectangle : public Element {
public:	
	Rectangle(T* value, const string& name);
	~Rectangle();

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
	
	Rectangle<T>& setColor(const float hue, float a = 1.0);
	Rectangle<T>& setX(const float min, const float max); // set min/max x
	Rectangle<T>& setY(const float min, const float max); // set min/max y
	void hide();
	void show();
	
private:
	void calculateCoordinates();
	bool near_tl;
	bool near_br;

public:
	
  	int label_dx;
	T* value;
	
	// calcs

	T min_x_value;
	T max_x_value;
	T min_y_value;
	T max_y_value;

	float perc_top_left[2];
	float perc_bottom_right[2]; // coordinates in gui in percentages
	
	// absolute x/y coordinates of rect in gui.
	float tl_x;
	float tl_y;
	float br_x;
	float br_y; 

	/*
	float px; // percentage x
	float py; // percentage y
	float point_x; 
	float point_y; 
	*/

	// colors
	float rect_bg_top_col[4];
	float rect_bg_bottom_col[4];
	float rect_line_col[4];
	float rect_dot_col[4]; // @todo remove if not used
	float rect_handle_active_col[4];
	float rect_handle_inactive_col[4]; 
	float* rect_tl_col;
	float* rect_br_col;

	// Used for drawing
	float bg_x;
	float bg_y; 
	float bg_h;
	float bg_w; 
};

	template<class T>
	Rectangle<T>::Rectangle(T* value, const string& name)
		:Element(BTYPE_RECTANGLE, name)
		,label_dx(0)
		,bg_x(0)
		,bg_y(0)
		,bg_h(0)
		,bg_w(0)
		,near_tl(false)
		,near_br(false)

		,min_x_value(0)
		,max_x_value(0)
		,min_y_value(0)
		,max_y_value(0)
		 /*
		,px(0.5f)
		,py(0.5f)
		 */
		,value(value)
		,rect_tl_col(NULL)
		,rect_br_col(NULL)
	{
		this->h = 120;
		perc_top_left[0] = perc_top_left[1] = 0.1f;
		perc_bottom_right[0] = perc_bottom_right[1] = 0.9f;
		rect_tl_col = rect_handle_inactive_col;
		rect_br_col = rect_handle_inactive_col;
	}

	template<class T>
	Rectangle<T>::~Rectangle() {
	}

	template<class T>
	void Rectangle<T>::generateStaticText() {
		label_dx = static_text->add(x+4, y+5, label, 0.9, 0.9, 0.9, 0.9);
	}

	template<class T>
	void Rectangle<T>::updateTextPosition() {
		static_text->setTextPosition(label_dx, this->x+4, this->y+5);
	}

	template<class T>
	void Rectangle<T>::generateVertices(ButtonVertices& vd) {
		calculateCoordinates();
		printf("..\n");
		/*
	  point_x = pad_x + px * pad_w;
		point_y = pad_y + py * pad_h;
		*/


		buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color); // background
		buttons::createRect(vd, bg_x, bg_y, bg_w, bg_h, this->rect_bg_top_col, this->rect_bg_bottom_col); // positioning area

		// Lines of the rectangular area
		vector<ButtonVertex> rect_verts;
		ButtonVertex vtl = ButtonVertex(tl_x, tl_y, rect_line_col);
		ButtonVertex vtr = ButtonVertex(br_x, tl_y, rect_line_col);
		ButtonVertex vbr = ButtonVertex(br_x, br_y, rect_line_col);
		ButtonVertex vbl = ButtonVertex(tl_x, br_y, rect_line_col);
		rect_verts.push_back(vtl);
		rect_verts.push_back(vtr);
		rect_verts.push_back(vbr);
		rect_verts.push_back(vbl);
		vd.add(rect_verts, GL_LINE_LOOP);

		// Highlight the corner
		{
			float handle_rect_thickness = 4;
			float handle_rect_size = 10;
		
			// top left corner
			buttons::createRect(vd, tl_x - 2, tl_y - 2, handle_rect_size, handle_rect_thickness, rect_tl_col, rect_tl_col);
			buttons::createRect(vd, tl_x - 2, tl_y - 2, handle_rect_thickness, handle_rect_size, rect_tl_col, rect_tl_col);
			
			// bottom right corner 
			buttons::createRect(vd, br_x - 2, br_y - (handle_rect_size-2), handle_rect_thickness, handle_rect_size, rect_br_col, rect_br_col);
			buttons::createRect(vd, br_x - (handle_rect_size-2), br_y - 2, handle_rect_size,handle_rect_thickness,  rect_br_col, rect_br_col);
		}

		// Lines towards the point.
		/*
		vector<ButtonVertex> line_verts;
		line_verts.push_back(ButtonVertex(this->pad_x, this->point_y, pad_line_col));
		line_verts.push_back(ButtonVertex(this->pad_x+pad_w, this->point_y, pad_line_col));
		line_verts.push_back(ButtonVertex(this->point_x, this->pad_y, pad_line_col));
		line_verts.push_back(ButtonVertex(this->point_x, this->pad_y +this->pad_h, pad_line_col));
		vd.add(line_verts, GL_LINES);
		*/

		// The point
		//buttons::createCircle(vd, point_x, point_y, 3.0f, pad_dot_col);
	}

	template<class T>
	void Rectangle<T>::onMouseMoved(int mx, int my) {
		if(!near_tl &&  !near_br) {
			return;
		}
				float local_x = float(mx) - bg_x;
				float local_y = float(my) - bg_y;
				
				local_x = std::max<float>(0.0f, std::min<float>(bg_w, local_x));
				local_y = std::max<float>(0.0f, std::min<float>(bg_h, local_y));

				float local_xp = local_x / bg_w;
				float local_yp = local_y / bg_h;
				if(near_tl) {
					perc_top_left[0] = local_xp;
					perc_top_left[1] = local_yp;
				}
				else {
					perc_bottom_right[0] = local_xp;
					perc_bottom_right[1] = local_yp;
				}
				calculateCoordinates();
				needsRedraw();

	}

	template<class T>
	void Rectangle<T>::onMouseDragged(int mx, int my, int dx, int dy) {
		if(drag_inside && !near_tl && !near_br) {
			printf("DRAG INSIDE!\n");
			// Get the closest handle
			float min_dist = 15;
			min_dist *= min_dist;

			float dx = (tl_x - mx);
			float dy = (tl_y - my);
			float tl_sq = (dx * dx) + (dy * dy);

			dx = (br_x - mx);
			dy = (br_y - my);
			float br_sq = (dx * dx) + (dy * dy);

			near_tl = false;
			near_br = false;
			if(tl_sq < min_dist) {
				near_tl = true;
			}
			else if(br_sq < min_dist) {
				near_br = true;
			}

			// Get local position
			if(near_tl || near_br) {
				/*
				float local_x = float(mx) - bg_x;
				float local_y = float(my) - bg_y;
				
				local_x = std::max<float>(0.0f, std::min<float>(bg_w, local_x));
				local_y = std::max<float>(0.0f, std::min<float>(bg_h, local_y));

				float local_xp = local_x / bg_w;
				float local_yp = local_y / bg_h;
				if(near_tl) {
					perc_top_left[0] = local_xp;
					perc_top_left[1] = local_yp;
				}
				else {
					perc_bottom_right[0] = local_xp;
					perc_bottom_right[1] = local_yp;
				}
				calculateCoordinates();
				needsRedraw();
				*/
			}
			
			//		printf("localx: %f, localy: %f, tr_x: %f\n", local_x, local_y, tl_x);
			/*
			px = std::max<float>(0.0f, std::min<float>(1.0f, local_xp));
			py = std::max<float>(0.0f, std::min<float>(1.0f, local_yp));

			*value = px * (max_x_value - min_x_value);
			*(value+1) = py * (max_y_value - min_y_value);
			needsRedraw();
			*/
		}
	}

	// Based on the perc_top_left and perc_bottom_right values we 
	// calculate the absolute screen coordinates which are used to 
	// draw the rectangle.
	template<class T>
	void Rectangle<T>::calculateCoordinates() {
		bg_w = this->w - 10;
		bg_h = this->h - 25;
		bg_x = this->x + (this->w - bg_w) / 2;
		bg_y = this->y + 20;

		printf("Topleft[0]: %f, topleft[1]: %f\n", perc_top_left[0], perc_top_left[1]);
		tl_x = bg_x + perc_top_left[0] * bg_w;
		tl_y = bg_y + perc_top_left[1] * bg_h;
		br_x = bg_x + perc_bottom_right[0] * bg_w;
		br_y = bg_y + perc_bottom_right[1] * bg_h;
	}

	template<class T>
	void Rectangle<T>::onMouseDown(int mx, int my) {
	}

	template<class T>
	void Rectangle<T>::onMouseUp(int mx, int my) {
		near_tl = false;
		near_br = false;
	}

	template<class T>
	void Rectangle<T>::onMouseEnter(int mx, int my) {
	}

	template<class T>
	void Rectangle<T>::onMouseLeave(int mx, int my) {
		//	near_tl = false;
		//	near_br = false;
	}

	template<class T>
	void Rectangle<T>::onMouseClick(int mx, int my) {
	}

	template<class T>
	void Rectangle<T>::save(std::ofstream& ofs) {
		/*
		size_t data_size = sizeof(T) * 2;
		ofs.write((char*)&data_size, sizeof(size_t)); // necessary !! (used to skip data when we remove an element)
		ofs.write((char*)value, sizeof(T) * 2);
		*/
	}

	template<class T>
	void Rectangle<T>::load(std::ifstream& ifs) {
		/*
		ifs.read((char*)value, sizeof(T) * 2);
		px = float(*(value)) / (max_x_value - min_x_value);
		py = float(*(value+1)) / (max_y_value - min_y_value);
		needsRedraw();
		*/
	}

	template<class T>
	bool Rectangle<T>::canSave() {
		return false;
		//return true;
	}

	template<class T>
	Rectangle<T>& Rectangle<T>::setColor(const float hue, float a) {
		Element::setColor(hue, a);

		rect_bg_top_col[3] = a;
		rect_bg_bottom_col[3] = a;
		HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, rect_bg_top_col, rect_bg_top_col+1, rect_bg_top_col+2);
		HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, rect_bg_bottom_col, rect_bg_bottom_col+1, rect_bg_bottom_col+2);
		HSL_to_RGB(col_hue, col_sat, col_bright + 0.2, rect_line_col, rect_line_col+1, rect_line_col+2);
		BSET_COLOR(rect_line_col, 1.0f, 1.0f, 1.0f, 0.3f);
		BSET_COLOR(rect_dot_col, 1.0f, 1.0f, 1.0f, 1.0f);

		//HSL_to_RGB(col_hue, col_sat, col_bright + 0.2, rect_handle_active_col, rect_handle_active_col+1, rect_handle_active_col+2);
		//HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, rect_handle_inactive_col, rect_handle_inactive_col+1, rect_handle_inactive_col+2);
		BSET_COLOR(rect_handle_active_col, 1.0f, 1.0f, 1.0f, 1.0f);
		BSET_COLOR(rect_handle_inactive_col, 0.8f, 0.8f, 0.8f, 1.0f);
		//rect_handle_active_col[3] = a;
		//rect_handle_inactive_col[3] = a;
		return *this;
	}

	template<class T>
	void Rectangle<T>::hide() {
		this->is_visible = false;
		this->static_text->setTextVisible(label_dx, false);
	}

	template<class T>
	void Rectangle<T>::show() {
		this->is_visible = true;
		this->static_text->setTextVisible(label_dx, true);
	}
	
	template<class T>
	Rectangle<T>& Rectangle<T>::setX(const float min, const float max) {
		min_x_value = min;
		max_x_value = max;
		return *this;
	}

	template<class T>
	Rectangle<T>& Rectangle<T>::setY(const float min, const float max) {
		min_y_value = min;
		max_y_value = max;
		return *this;
	}


} // namespace buttons

#endif
