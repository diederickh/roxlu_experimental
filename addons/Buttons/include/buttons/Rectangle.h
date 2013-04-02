#ifndef ROXLU_RECTANGLEH
#define ROXLU_RECTANGLEH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {
  enum RectangleValueType {
    RECT_INT,
    RECT_FLOAT
  };

  template<class T>
    class Rectangle : public Element {
  public:	
    Rectangle(T* value, const string& name, RectangleValueType valueType);
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
    void save(config_setting_t* setting);
    void load(std::ifstream& ifs);
    void load(config_setting_t* setting);
    bool canSave();
	
    Rectangle<T>& setColor(const float hue, float sat = 0.2f, float bright = 0.27f, float a = 1.0);
    Rectangle<T>& setX(const float min, const float max); // set min/max x
    Rectangle<T>& setY(const float min, const float max); // set min/max y
    Rectangle<T>& hide();
    Rectangle<T>& show();
	
  private:
    void calculateCoordinates();
    float calculatePercentage(float value, float min, float maxv);
    bool near_tl;
    bool near_br;
    bool inside_rect;

  public:
    int value_type;
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

    // colors
    float rect_bg_top_col[4];
    float rect_bg_bottom_col[4];
    float rect_line_col[4];
    float rect_handle_active_col[4];
    float rect_handle_inactive_col[4]; 
    float rect_drag_col[4];
    float* rect_tl_col;
    float* rect_br_col;
    float* rect_selected_line_col;

    // Used for drawing
    float bg_x;
    float bg_y; 
    float bg_h;
    float bg_w; 
  };

  template<class T>
    Rectangle<T>::Rectangle(T* value, const string& name, RectangleValueType valueType )
    :Element(BTYPE_RECTANGLE, name)
    ,label_dx(0)
    ,value_type(valueType)
    ,bg_x(0)
    ,bg_y(0)
    ,bg_h(0)
    ,bg_w(0)
    ,near_tl(false)
    ,near_br(false)
    ,inside_rect(false)
    ,min_x_value(0)
    ,max_x_value(0)
    ,min_y_value(0)
    ,max_y_value(0)
    ,value(value)
    ,rect_tl_col(NULL)
    ,rect_br_col(NULL)
    ,tl_x(0)
    ,tl_y(0)
    ,br_x(0)
    ,br_y(0)
    {
      this->h = 120;
      perc_top_left[0] = perc_top_left[1] = 0.1f;
      perc_bottom_right[0] = perc_bottom_right[1] = 0.9f;
      rect_tl_col = rect_handle_inactive_col;
      rect_br_col = rect_handle_inactive_col;
      rect_selected_line_col = rect_handle_inactive_col;;
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

    // Background + pad area
    buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color); // background
    buttons::createRect(vd, bg_x, bg_y, bg_w, bg_h, this->rect_bg_top_col, this->rect_bg_bottom_col); // positioning area

    // Lines of the rectangular area
    vector<ButtonVertex> rect_verts;
    ButtonVertex vtl = ButtonVertex(tl_x, tl_y, rect_selected_line_col);
    ButtonVertex vtr = ButtonVertex(br_x, tl_y, rect_selected_line_col);
    ButtonVertex vbr = ButtonVertex(br_x, br_y, rect_selected_line_col);
    ButtonVertex vbl = ButtonVertex(tl_x, br_y, rect_selected_line_col);
    rect_verts.push_back(vtl);
    rect_verts.push_back(vtr);
    rect_verts.push_back(vbr);
    rect_verts.push_back(vbl);
    vd.add(rect_verts, GL_LINE_LOOP);

    // When dragging inside the rect, show background.
    if(inside_rect) {
      buttons::createRect(vd, tl_x, tl_y, (br_x - tl_x), (br_y - tl_y), rect_drag_col, rect_drag_col);
    }

    // Highlight the two corners
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
  }

  template<class T>
    void Rectangle<T>::onMouseMoved(int mx, int my) {
    if(drag_inside && !near_tl && !near_br) {
      // Get the closest handle
      float min_dist = 5;
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
        rect_tl_col = rect_handle_active_col;
        rect_br_col = rect_handle_inactive_col;
      }
      else if(br_sq < min_dist) {
        near_br = true;
        rect_tl_col = rect_handle_inactive_col;
        rect_br_col = rect_handle_active_col;
      }
      else if(BMOUSE_INSIDE(mx,my,tl_x, tl_y, (br_x - tl_x), (br_y - tl_y))) {
        inside_rect = true;
      }
    }
		
    if(!near_tl &&  !near_br && !inside_rect) {
      return;
    }
		
    if(inside_rect) {
      rect_selected_line_col = rect_handle_active_col;
    }

    float local_x = float(mx) - bg_x;
    float local_y = float(my) - bg_y;
    local_x = std::max<float>(0.0f, std::min<float>(bg_w, local_x));
    local_y = std::max<float>(0.0f, std::min<float>(bg_h, local_y));

    if(near_tl || near_br) {
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
    }
    else {
      // inside rectangular area, calc displacement to make center of rect the mouse position
      float area_w = br_x - tl_x;
      float area_h = br_y - tl_y;
      float new_tl_x = local_x - (area_w * 0.5);
      float new_tl_y = local_y - (area_h * 0.5);
      float new_br_x = new_tl_x + area_w;
      float new_br_y = new_tl_y + area_h;
      perc_top_left[0] = new_tl_x / bg_w;
      perc_top_left[1] = new_tl_y / bg_h;
      perc_bottom_right[0] = new_br_x / bg_w;
      perc_bottom_right[1] = new_br_y / bg_h;
    }
    calculateCoordinates();
    needsRedraw();

  }

  template<class T>
    void Rectangle<T>::onMouseDragged(int mx, int my, int dx, int dy) {
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

    perc_top_left[0] = BLIMIT_FLOAT(perc_top_left[0], 0.0f, 1.0f);
    perc_top_left[1] = BLIMIT_FLOAT(perc_top_left[1], 0.0f, 1.0f);
    perc_bottom_right[0] = BLIMIT_FLOAT(perc_bottom_right[0], 0.0f, 1.0f);
    perc_bottom_right[1] = BLIMIT_FLOAT(perc_bottom_right[1], 0.0f, 1.0f);

    tl_x = bg_x + perc_top_left[0] * bg_w;
    tl_y = bg_y + perc_top_left[1] * bg_h;
    br_x = bg_x + perc_bottom_right[0] * bg_w;
    br_y = bg_y + perc_bottom_right[1] * bg_h;

    float yrange = max_y_value - min_y_value;
    float xrange = max_x_value - min_x_value;
    *(value) = min_x_value + perc_top_left[0] * xrange;
    *(value+1) = min_y_value + perc_top_left[1] * yrange;
    *(value+2) = min_x_value + perc_bottom_right[0] * xrange;
    *(value+3) = min_y_value + perc_bottom_right[1] * yrange;
  }

  template<class T>
    float Rectangle<T>::calculatePercentage(float value, float minv, float maxv) {
    float p = 1.0f/(maxv-minv) * value - (minv/(maxv-minv)); 
    p = BLIMIT_FLOAT(p, 0.0f, 1.0f);
    return p;
  }

  template<class T>
    void Rectangle<T>::onMouseDown(int mx, int my) {
  }

  template<class T>
    void Rectangle<T>::onMouseUp(int mx, int my) {
    near_tl = false;
    near_br = false;
    inside_rect = false;
    rect_tl_col = rect_handle_inactive_col;
    rect_br_col = rect_handle_inactive_col;
    rect_selected_line_col = rect_handle_inactive_col;
    needsRedraw();
  }

  template<class T>
    void Rectangle<T>::onMouseEnter(int mx, int my) {
  }

  template<class T>
    void Rectangle<T>::onMouseLeave(int mx, int my) {
  }

  template<class T>
    void Rectangle<T>::onMouseClick(int mx, int my) {
  }

  template<class T>
    void Rectangle<T>::save(std::ofstream& ofs) {
    size_t data_size = sizeof(T) * 4; // 4 items (top left x/y and bottom right x/y
    ofs.write((char*)&data_size, sizeof(size_t)); // necessary !! (used to skip data when we remove an element)
    ofs.write((char*)value, sizeof(T) * 4);
  }

  template<class T>
    void Rectangle<T>::save(config_setting_t* setting) {
    if(value_type == RECT_INT) {
      config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
      config_setting_set_int_elem(cfg_el, -1, value[0]);
      config_setting_set_int_elem(cfg_el, -1, value[1]);
      config_setting_set_int_elem(cfg_el, -1, value[2]);
      config_setting_set_int_elem(cfg_el, -1, value[3]);
    }
    else if(value_type == RECT_FLOAT) {
      config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
      config_setting_set_float_elem(cfg_el, -1, value[0]);
      config_setting_set_float_elem(cfg_el, -1, value[1]);
      config_setting_set_float_elem(cfg_el, -1, value[2]);
      config_setting_set_float_elem(cfg_el, -1, value[3]);
    }
  }

  template<class T>
    void Rectangle<T>::load(std::ifstream& ifs) {
    ifs.read((char*)value, sizeof(T) * 4);
    perc_top_left[0] = calculatePercentage(*(value), min_x_value, max_x_value);
    perc_top_left[1] = calculatePercentage(*(value+1), min_y_value, max_y_value);
    perc_bottom_right[0] = calculatePercentage(*(value+2), min_x_value, max_x_value);
    perc_bottom_right[1] = calculatePercentage(*(value+3), min_y_value, max_y_value);
    needsRedraw();
  }

  template<class T> 
    void Rectangle<T>::load(config_setting_t* setting) {
      std::string cname = buttons_create_clean_name(name);
      config_setting_t* cfg = config_setting_get_member(setting, cname.c_str());
      if(!cfg) {
        printf("ERROR: cannot find setting for: %s\n", cname.c_str());
        return;
      }
      if(value_type == RECT_INT) {
        value[0] = config_setting_get_int_elem(cfg, 0);
        value[1] = config_setting_get_int_elem(cfg, 1);
        value[2] = config_setting_get_int_elem(cfg, 2);
        value[3] = config_setting_get_int_elem(cfg, 3);
      }
      else if(value_type == RECT_FLOAT) {
        value[0] = config_setting_get_float_elem(cfg, 0);
        value[1] = config_setting_get_float_elem(cfg, 1);
        value[2] = config_setting_get_float_elem(cfg, 2);
        value[3] = config_setting_get_float_elem(cfg, 3);
      } 

      perc_top_left[0] = calculatePercentage(*(value), min_x_value, max_x_value);
      perc_top_left[1] = calculatePercentage(*(value+1), min_y_value, max_y_value);
      perc_bottom_right[0] = calculatePercentage(*(value+2), min_x_value, max_x_value);
      perc_bottom_right[1] = calculatePercentage(*(value+3), min_y_value, max_y_value);
  }

  template<class T>
    bool Rectangle<T>::canSave() {
    return true;
  }

  template<class T>
    Rectangle<T>& Rectangle<T>::setColor(const float hue, float sat, float bright, float a) {
    Element::setColor(hue, sat, bright, a);

    rect_bg_top_col[3] = a;
    rect_bg_bottom_col[3] = a;
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, rect_bg_top_col, rect_bg_top_col+1, rect_bg_top_col+2);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, rect_bg_bottom_col, rect_bg_bottom_col+1, rect_bg_bottom_col+2);
    HSL_to_RGB(col_hue, col_sat, col_bright + 0.2, rect_line_col, rect_line_col+1, rect_line_col+2);
    BSET_COLOR(rect_line_col, 1.0f, 1.0f, 1.0f, 0.3f);

    BSET_COLOR(rect_handle_active_col, 1.0f, 1.0f, 1.0f, 1.0f);
    BSET_COLOR(rect_handle_inactive_col, 1.0f, 1.0f, 1.0f, 0.5f);
    BSET_COLOR(rect_drag_col, 1.0f, 1.0f, 1.0f, 0.05f);
    return *this;
  }

  template<class T>
    Rectangle<T>& Rectangle<T>::hide() {
    this->is_visible = false;
    this->static_text->setTextVisible(label_dx, false);
    return *this;
  }

  template<class T>
    Rectangle<T>& Rectangle<T>::show() {
    this->is_visible = true;
    this->static_text->setTextVisible(label_dx, true);
    return *this;
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
