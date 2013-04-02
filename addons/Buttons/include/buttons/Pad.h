/** 
 * A 2D pad. Draw around to set a value.
 * If you want to change the ranges, call: setX() and setY()
 *
 *        Example:
 *        --------
 *        int my_pos[2];
 *        gui.addInt2("mypos", my_pos).setX(0,100).setY(0.600);
 *
 */
#ifndef ROXLU_PADH
#define ROXLU_PADH

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <libconfig.h>

namespace buttons {

  enum PadValueType {
     PAD_INT
    ,PAD_FLOAT
  };

  template<class T>
    class Pad : public Element {
  public:	
    Pad(T* value, const string& name, PadValueType type);
    ~Pad();

    void generateStaticText();
    void updateTextPosition();
    void generateVertices(ButtonVertices& shapeVertices);

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
    float calculatePercentage(float value, float minv, float maxv); 
    void calculateValues(); // calculate resluting x/y values;

    Pad<T>& setColor(const float hue, float sat = 0.2f, float bright = 0.27f, float a = 1.0);
    Pad<T>& setX(const float min, const float max);
    Pad<T>& setY(const float min, const float max);
    Pad<T>& setPercentages(const float percX, const float percY);

    void setValue(void* v); // client <-> server
    bool serializeScheme(ButtonsBuffer& buffer);

    Pad<T>& hide();
    Pad<T>& show();
	
    int label_dx;
    T* value;
    int value_type;
	
    // calcs
    T min_x_value;
    T max_x_value;
    T min_y_value;
    T max_y_value;
    float px; // percentage x
    float py; // percentage y
    float point_x; 
    float point_y; 

    // colors
    float pad_bg_top_col[4];
    float pad_bg_bottom_col[4];
    float pad_line_col[4];
    float pad_dot_col[4];

    // Used for drawing
    float pad_x;
    float pad_y; 
    float pad_h;
    float pad_w; 
  };

  template<class T>
    Pad<T>::Pad(T* value, const string& name, PadValueType type)
    :Element(BTYPE_PAD, name)
    ,label_dx(0)
    ,pad_x(0)
    ,pad_y(0)
    ,pad_h(0)
    ,pad_w(0)
    ,min_x_value(0)
    ,max_x_value(100)
    ,min_y_value(0)
    ,max_y_value(100)
    ,px(0.5f)
    ,py(0.5f)
    ,value(value)
    ,value_type(type)
  {
    this->h = 120;
  }

  template<class T>
    Pad<T>::~Pad() {
  }

  template<class T>
    void Pad<T>::generateStaticText() {
    label_dx = static_text->add(x+4, y+5, label, 0.9, 0.9, 0.9, 0.9);
  }

  template<class T>
    void Pad<T>::updateTextPosition() {
    static_text->setTextPosition(label_dx, this->x+4, this->y+5);
  }

  template<class T>
    void Pad<T>::generateVertices(ButtonVertices& vd) {
    pad_w = this->w - 10;
    pad_h = this->h - 25;
    pad_x = this->x + (this->w - pad_w) / 2;
    pad_y = this->y + 20;
    point_x = pad_x + px * pad_w;
    point_y = pad_y + py * pad_h;


    buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color); // background
    buttons::createRect(vd, pad_x, pad_y, pad_w, pad_h, this->pad_bg_top_col, this->pad_bg_bottom_col); // positioning area

    // Lines towards the point.
    vector<ButtonVertex> line_verts;
    line_verts.push_back(ButtonVertex(this->pad_x, this->point_y, pad_line_col));
    line_verts.push_back(ButtonVertex(this->pad_x+pad_w, this->point_y, pad_line_col));
    line_verts.push_back(ButtonVertex(this->point_x, this->pad_y, pad_line_col));
    line_verts.push_back(ButtonVertex(this->point_x, this->pad_y +this->pad_h, pad_line_col));

    vd.add(line_verts, GL_LINES);

    // The point
    buttons::createCircle(vd, point_x, point_y, 3.0f, pad_dot_col);
  }

  template<class T>
    void Pad<T>::onMouseDragged(int mx, int my, int dx, int dy) {
    if(drag_inside) {
      // Get local position
      float local_x = float(mx) - pad_x;
      float local_y = float(my) - pad_y;
      float local_xp = local_x / pad_w;
      float local_yp = local_y / pad_h;
      px = std::max<float>(0.0f, std::min<float>(1.0f, local_xp));
      py = std::max<float>(0.0f, std::min<float>(1.0f, local_yp));
      calculateValues();
      needsRedraw();
      flagValueChanged();
    }
  }


  template<class T>
    void Pad<T>::onMouseDown(int mx, int my) {
  }

  template<class T>
    void Pad<T>::onMouseUp(int mx, int my) {
  }

  template<class T>
    void Pad<T>::onMouseEnter(int mx, int my) {
  }

  template<class T>
    void Pad<T>::onMouseLeave(int mx, int my) {
  }

  template<class T>
    void Pad<T>::onMouseClick(int mx, int my) {
  }

  template<class T>
    void Pad<T>::calculateValues() {
    *value = px * (max_x_value - min_x_value);
    *(value+1) = py * (max_y_value - min_y_value);
  }

  template<class T>
    float Pad<T>::calculatePercentage(float value, float minv, float maxv) {
    float p = 1.0f/(maxv-minv) * value - (minv/(maxv-minv)); 
    p = BLIMIT_FLOAT(p, 0.0f, 1.0f);
    return p;
  }

  template<class T>
    void Pad<T>::save(std::ofstream& ofs) {
    size_t data_size = sizeof(T) * 2;
    ofs.write((char*)&data_size, sizeof(size_t)); // necessary !! (used to skip data when we remove an element)
    ofs.write((char*)value, sizeof(T) * 2);
  }

  template<class T> 
    void Pad<T>::save(config_setting_t* setting) {
    if(value_type == PAD_INT) {
      config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
      config_setting_set_int_elem(cfg_el, -1, value[0]);
      config_setting_set_int_elem(cfg_el, -1, value[1]);
    }
    else if(value_type == PAD_FLOAT) {
      config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
      config_setting_set_float_elem(cfg_el, -1, value[0]);
      config_setting_set_float_elem(cfg_el, -1, value[1]);
    }
  }

  template<class T>
    void Pad<T>::load(std::ifstream& ifs) {
    ifs.read((char*)value, sizeof(T) * 2);
    px = calculatePercentage(float(*(value)), min_x_value, max_x_value);
    py = calculatePercentage(float(*(value+1)), min_y_value, max_y_value);
    needsRedraw();
  }
  
  template<class T> 
    void Pad<T>::load(config_setting_t* setting) {
    std::string cname = buttons_create_clean_name(name);

    config_setting_t* cfg = config_setting_get_member(setting, cname.c_str());
    if(!cfg) {
      printf("ERROR: cannot find pad setting.\n");
      return;
    }

    if(value_type == PAD_INT) {
      value[0] = config_setting_get_int_elem(cfg, 0);
      value[1] = config_setting_get_int_elem(cfg, 1);
    }
    else if(value_type == PAD_FLOAT) {
      value[0] = config_setting_get_float_elem(cfg, 0);
      value[1] = config_setting_get_float_elem(cfg, 1);
    }

    px = calculatePercentage(float(*(value)), min_x_value, max_x_value);
    py = calculatePercentage(float(*(value+1)), min_y_value, max_y_value);
    needsRedraw();
  }

  // Set value by void*, must be a float*
  template<class T>
    void Pad<T>::setValue(void* v) {
    float* percp = (float*)v;
    setPercentages(percp[0], percp[1]);
    calculateValues();
    needsRedraw();
  }

  template<class T>
    Pad<T>& Pad<T>::setPercentages(const float percX, const float percY) {
    px = percX;
    py = percY;
    return *this;
  }

  template<class T>
    bool Pad<T>::canSave() {
    return true;
  }

  template<class T>
    Pad<T>& Pad<T>::setColor(const float hue, float sat, float bright, float a) {
    Element::setColor(hue, sat, bright, a);

    pad_bg_top_col[3] = a;
    pad_bg_bottom_col[3] = a;
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, pad_bg_top_col, pad_bg_top_col+1, pad_bg_top_col+2);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, pad_bg_bottom_col, pad_bg_bottom_col+1, pad_bg_bottom_col+2);
    HSL_to_RGB(col_hue, col_sat, col_bright + 0.2, pad_line_col, pad_line_col+1, pad_line_col+2);
    BSET_COLOR(pad_line_col, 1.0f, 1.0f, 1.0f, 0.3f);
    BSET_COLOR(pad_dot_col, 1.0f, 1.0f, 1.0f, 1.0f);
    return *this;
  }

  template<class T>
    Pad<T>& Pad<T>::hide() {
    this->is_visible = false;
    this->static_text->setTextVisible(label_dx, false);
    return *this;
  }

  template<class T>
    Pad<T>& Pad<T>::show() {
    this->is_visible = true;
    this->static_text->setTextVisible(label_dx, true);
    return *this;
  }
	
  template<class T>
    Pad<T>& Pad<T>::setX(const float min, const float max) {
    min_x_value = min;
    max_x_value = max;
    return *this;
  }

  template<class T>
    Pad<T>& Pad<T>::setY(const float min, const float max) {
    min_y_value = min;
    max_y_value = max;
    return *this;
  }

  template<class T>
    bool Pad<T>::serializeScheme(ButtonsBuffer& buffer) {
    if(value_type == PAD_FLOAT) {
      buffer.addByte(PAD_FLOAT);
      buffer.addFloat(min_x_value);
      buffer.addFloat(max_x_value);
      buffer.addFloat(min_y_value);
      buffer.addFloat(max_y_value);
    }
    else {
      buffer.addByte(PAD_INT);
      buffer.addUI32(min_x_value);
      buffer.addUI32(max_x_value);
      buffer.addUI32(min_y_value);
      buffer.addUI32(max_y_value);
    }
    buffer.addFloat(px);
    buffer.addFloat(py);
    return true;
  }

} // namespace buttons

#endif
