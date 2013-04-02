/**
 * Vector2 element
 * ---------------
 * Creates a vector2 that you can drag around to 
 * e.g. make a directional force. The vector will always
 * be normalized, so you need to scale it manually.
 * 
 * 
 *    Example:
 *    --------
 *    float vec[2];
 *    gui.addVec2f("Direction", vec);
 * 
 */
#ifndef ROXLU_VECTORH
#define ROXLU_VECTORH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {
  
  enum VectorValueType {
    VEC_INT,
    VEC_FLOAT
  };

  template<class T>
    class Vector : public Element {
  public:	
    Vector(T* value, const string& name, VectorValueType valueType);
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
    void save(config_setting_t* setting);
    void load(std::ifstream& ifs);
    void load(config_setting_t* setting);
    bool canSave();
	
    void setValue(void* v); // used by e.g. client<->server
    bool serializeScheme(ButtonsBuffer& buffer);
	
    Vector<T>& setColor(const float hue, float sat = 0.2f , float bright = 0.27f, float a = 1.0);
    Vector<T>& hide();
    Vector<T>& show();
	
  private:
    void calculateCoordinates();

    float circle_bg_col[4];
    float rect_bg_top_col[4];
    float rect_bg_bottom_col[4];
    float arrow_fill_col[4];

    float* arrow_col;
    float radius; 
    float cx;
    float cy;
    float angle;
    float cos_a;
    float sin_a; 
    bool is_rotating;

  public:
    int value_type;
    int label_dx;
    T* value;

    // Used for drawing
    float bg_x;
    float bg_y; 
    float bg_h;
    float bg_w; 
  };

  template<class T>
    Vector<T>::Vector(T* value, const string& name, VectorValueType valueType)
    :Element(BTYPE_VECTOR, name)
    ,label_dx(0)
    ,value_type(valueType)
    ,bg_x(0)
    ,bg_y(0)
    ,bg_h(0)
    ,bg_w(0)
    ,value(value)
    ,radius(1.0f)
    ,cx(0)
    ,cy(0)
    ,angle(0)
    ,is_rotating(false)
    ,arrow_col(NULL)
    ,cos_a(0.0f)
    ,sin_a(0.0f)
    {
      this->h = 120;
      setColor(1.0f); 
      event_data = (void*)&value;
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
    //float end_x = cx + cos(angle) * (radius * 0.7);
    //float end_y = cy + sin(angle) * (radius * 0.7);
    float end_x = cx + cos_a * (radius * 0.7);
    float end_y = cy + sin_a * (radius * 0.7);
		
    float dir_x = end_x - cx;
    float dir_y = end_y - cy;
    float dx = (dir_x * dir_x);
    float dy = (dir_y * dir_y);
    float len = sqrtf(dx + dy);

    dir_y /= len;
    dir_x /= len;

    float perp_x = -dir_y;
    float perp_y = dir_x;
		
    float point_w = 9.0f;
    float arrow_w = 2.5f;

    // a = bottom left corner
    // b = bottom right corner
    // c = top left corner
    // d = top right corner
    // e = left arrow point
    // f = right arrow point
    // g = top arrow point 
    ButtonVertex v_a(cx - (perp_x * arrow_w), cy - (perp_y * arrow_w), arrow_col);
    ButtonVertex v_b(cx + (perp_x * arrow_w), cy + (perp_y * arrow_w), arrow_col);
    ButtonVertex v_c(end_x - (perp_x * arrow_w), end_y - (perp_y * arrow_w), arrow_col);
    ButtonVertex v_d(end_x + (perp_x * arrow_w), end_y + (perp_y * arrow_w), arrow_col);
    ButtonVertex v_e(end_x - (perp_x * point_w), end_y - (perp_y * point_w), arrow_col);
    ButtonVertex v_f(end_x + (perp_x * point_w), end_y + (perp_y * point_w), arrow_col);
    ButtonVertex v_g(cx + (dir_x * len * 1.4), cy + (dir_y * len * 1.4), arrow_col);

    vector<ButtonVertex> verts;
    verts.push_back(v_a);
    verts.push_back(v_b);
    verts.push_back(v_d);
    verts.push_back(v_a);
    verts.push_back(v_d);
    verts.push_back(v_c);
    verts.push_back(v_f);
    verts.push_back(v_g);
    verts.push_back(v_e);
    vd.add(verts, GL_TRIANGLES);
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
      cos_a = cos(angle);
      sin_a = sin(angle);
      *value = cos_a;
      *(value+1) = sin_a;
      event_data = (void*)value;
      //			float* f = (float*)event_data;
      //			printf("value p: %p/%p, value[0]: %f, event_data[0]: %f\n", value, event_data, value[0], *f);
      needsRedraw();
      flagValueChanged();
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
    size_t data_size = sizeof(T) * 2; // 4 items (top left x/y and bottom right x/y
    ofs.write((char*)&data_size, sizeof(size_t)); // necessary !! (used to skip data when we remove an element)
    ofs.write((char*)value, sizeof(T) * 2);
  }

  template<class T>
    void Vector<T>::save(config_setting_t* setting) {
    if(value_type == VEC_INT) {
      config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
      config_setting_set_int_elem(cfg_el, -1, value[0]);
      config_setting_set_int_elem(cfg_el, -1, value[1]);
    }
    else if(value_type == VEC_FLOAT) {
      config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
      config_setting_set_float_elem(cfg_el, -1, value[0]);
      config_setting_set_float_elem(cfg_el, -1, value[1]);      
    }
  }

  template<class T>
    void Vector<T>::load(std::ifstream& ifs) {
    ifs.read((char*)value, sizeof(T) * 2);
    cos_a = *value;
    sin_a = *(value+1);
    needsRedraw();
  }

  template<class T>
    void Vector<T>::load(config_setting_t* setting) {
    std::string cname = buttons_create_clean_name(name);
    config_setting_t* cfg = config_setting_get_member(setting, cname.c_str());
    if(!cfg) {
      printf("ERROR: cannot find setting for: %s\n", cname.c_str());
      return;
    }

    if(value_type == VEC_INT) {
      printf("ERROR: we did not yet implement the INT vec type for loading.\n");
    }
    else if(value_type == VEC_FLOAT) {
      double tmp_value = 0;
      value[0] = config_setting_get_float_elem(cfg, 0);
      value[1] = config_setting_get_float_elem(cfg, 1);
      cos_a = *value;
      sin_a = *(value+1);
    }

    needsRedraw();
    needsTextUpdate();

  }

  template<class T>
    bool Vector<T>::canSave() {
    return true;
  }

  template<class T>
    Vector<T>& Vector<T>::setColor(const float hue, float sat, float bright, float a) {
    Element::setColor(hue, sat, bright, a);
    rect_bg_top_col[3] = a;
    rect_bg_bottom_col[3] = a;
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, rect_bg_top_col, rect_bg_top_col+1, rect_bg_top_col+2);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, rect_bg_bottom_col, rect_bg_bottom_col+1, rect_bg_bottom_col+2);
    BSET_COLOR(circle_bg_col, 1.0f, 1.0f, 1.0f, 0.05);
    BSET_COLOR(arrow_fill_col, 1.0f, 1.0f, 1.0f, 1.0f);
    arrow_col = arrow_fill_col;
    return *this;
  }

  template<class T>
    void Vector<T>::setValue(void* v) {

    T* vp = (T*)v;
    value[0] = vp[0];
    value[1] = vp[1];
    cos_a = value[0];
    sin_a = value[1];
    needsRedraw();
  }

  template<class T>
    Vector<T>& Vector<T>::hide() {
    this->is_visible = false;
    this->static_text->setTextVisible(label_dx, false);
    return *this;
  }

  template<class T>
    Vector<T>& Vector<T>::show() {
    this->is_visible = true;
    this->static_text->setTextVisible(label_dx, true);
    return *this;
  }

  template<class T>
    bool Vector<T>::serializeScheme(ButtonsBuffer& buffer) {
    buffer.addFloat(cos_a);
    buffer.addFloat(sin_a);
    return true;
  }

} // namespace buttons

#endif
