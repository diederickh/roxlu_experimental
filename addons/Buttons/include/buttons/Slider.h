#ifndef ROXLU_BSLIDERH
#define ROXLU_BSLIDERH

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <sstream>
#include <libconfig.h>

namespace buttons {

  inline void set_slider_step_value(int& result) {
    result = 1;
  }
  inline void set_slider_step_value(float& result) {
    result = 0.01;
  }

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
      ,bt_dec_x(0)
      ,bt_inc_x(0)
      ,bt_y(0)
      ,bt_w(0)
      ,bt_h(0)
      ,bt_inc_top_color_ptr(NULL)
      ,bt_inc_bottom_color_ptr(NULL)
      ,bt_dec_top_color_ptr(NULL)
      ,bt_dec_bottom_color_ptr(NULL)
      ,stepv(0)
      ,bt_mouse_down(-1)
      {
        h = 22;
        bt_inc_top_color_ptr = bt_top_col;
        bt_inc_bottom_color_ptr = bt_bottom_col;
        bt_dec_top_color_ptr = bt_top_col;
        bt_dec_bottom_color_ptr = bt_bottom_col;
        set_slider_step_value(stepv);
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
	
    Slider& setColor(const float hue, float sat, float bright, float a = 1.0) {
      Element::setColor(hue, sat, bright, a);
		
      bar_empty_color[3] = a;
      bar_filled_color[3] = a;
      bar_filled_bottom[3] = a;
      bt_top_col[3] = a;
      bt_bottom_col[3] = a;

      HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, &bar_empty_color[0], &bar_empty_color[1], &bar_empty_color[2]);
      HSL_to_RGB(col_hue, col_sat, col_bright + 0.4, &bar_filled_color[0], &bar_filled_color[1], &bar_filled_color[2]);
      HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, &bar_filled_bottom[0], &bar_filled_bottom[1], &bar_filled_bottom[2]);
      HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, bt_top_col, bt_top_col+1, bt_top_col+2);
      HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, bt_bottom_col, bt_bottom_col+1, bt_bottom_col+2);
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
      dynamic_text->setTextAlign(txtval_dx, TEXT_ALIGN_RIGHT, (w - 50));
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
      bt_w = 20;
		
      int bar_h = 4; // bar height
      int bar_filled_w = (w - 2 * bt_w) * p;
      int bar_empty_w = (w - 2 * bt_w) * (1.0 - p);
      int bar_x = x; // start x
      int bar_y = (y + h) - bar_h; // start y

      // button + / -
      bt_dec_x = bar_x + (w - (bt_w * 2) );
      bt_inc_x = bar_x + (w - bt_w);
      bt_y = y;
      bt_h = h; // - bar_h;

      buttons::createRect(vd, bt_dec_x, bt_y, bt_w, bt_h, bt_dec_top_color_ptr, bt_dec_bottom_color_ptr); // dec bg
      buttons::createRect(vd, bt_inc_x, bt_y, bt_w, bt_h, bt_inc_top_color_ptr, bt_inc_bottom_color_ptr); // inc bg
      buttons::createRect(vd, bt_inc_x + bt_w * 0.2, bt_y + bt_h * 0.45, bt_w * 0.6, 2, col_text, col_text); // + sign
      buttons::createRect(vd, bt_inc_x + bt_w * 0.45, bt_y + bt_h * 0.20, 2, bt_h * 0.6, col_text, col_text); // + sign		
      buttons::createRect(vd, bt_dec_x + bt_w * 0.2, bt_y + bt_h * 0.45, bt_w * 0.6, 2, col_text, col_text); // - sign

      // slider
      buttons::createRect(vd, bar_x, bar_y, bar_filled_w, bar_h, bar_filled_color, bar_filled_bottom);
      buttons::createRect(vd, bar_x + bar_filled_w, bar_y, bar_empty_w, bar_h, bar_empty_color, bar_empty_color);
    }

    void update() {
      // when you press the + or -, inc/dec
      if(bt_mouse_down == 0) {
        setValue(value + stepv);
        needsRedraw();
        bt_mouse_down = -1;
      }
      else if(bt_mouse_down == 1) {
        setValue(value - stepv);
        needsRedraw();
        bt_mouse_down = -1;
      }
    }

    void onMouseDown(int mx, int my) {
      // did we press one of the inc/dec buttons?
      if(BMOUSE_INSIDE(mx,my,bt_inc_x, bt_y, bt_w, bt_h)) {
        bt_mouse_down = 0;
      }
      else if(BMOUSE_INSIDE(mx,my,bt_dec_x, bt_y, bt_w, bt_h)) {
        bt_mouse_down = 1;
      }
      else {
        bt_mouse_down = -1;
      }
    }

    void onMouseUp(int mx, int my) {

      if(!is_mouse_inside) { // mouse up after
        bg_top_color = col_bg_default;
        bg_bottom_color = col_bg_default;
        needsRedraw();
      }
      else {
        // did we press one of the inc/dec buttons?
        if(bt_mouse_down == 0 && BMOUSE_INSIDE(mx,my,bt_inc_x, bt_y, bt_w, bt_h)) {
          setValue(value + stepv);
          needsRedraw();
          needsTextUpdate();
          flagValueChanged();
        }
        else if(bt_mouse_down == 1 && BMOUSE_INSIDE(mx,my,bt_dec_x, bt_y, bt_w, bt_h)) {
          setValue(value - stepv);
          needsRedraw();
          needsTextUpdate();
          flagValueChanged();
        }
      }
      bt_mouse_down = -1;
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

        if(BMOUSE_INSIDE(mx, my, bt_dec_x, y, bt_dec_x + bt_w * 2, bt_h)) { 
          return;
        }

        int range_w = (w - (bt_w * 2));
        int local_x = std::min<int>(x+range_w,std::max<int>(x,mx));
        p = (float)(local_x-x)/range_w;
        value = minv + (p * (maxv-minv));
        needsRedraw();
        needsTextUpdate();
        flagValueChanged();
      }
    }

    void onMouseMoved(int mx, int my) {
      float* curr_inc = bt_inc_top_color_ptr;
      float* curr_dec = bt_dec_top_color_ptr;

      // check increment button
      if(BMOUSE_INSIDE(mx,my,bt_inc_x, bt_y, bt_w, bt_h)) {
        bt_inc_top_color_ptr = bt_bottom_col;
        bt_inc_bottom_color_ptr = bt_top_col;
      }
      else {
        bt_inc_top_color_ptr = bt_top_col;
        bt_inc_bottom_color_ptr = bt_bottom_col; //bt_bottom_col;
      }

      // check decrement button
      if(BMOUSE_INSIDE(mx,my,bt_dec_x, bt_y, bt_w, bt_h)) {
        bt_dec_top_color_ptr = bt_bottom_col;
        bt_dec_bottom_color_ptr = bt_top_col;
      }
      else {
        bt_dec_top_color_ptr = bt_top_col;
        bt_dec_bottom_color_ptr = bt_bottom_col; //bt_bottom_col;
      }

      // when colors changes we redraw
      if(bt_inc_top_color_ptr != curr_inc 
         || bt_dec_top_color_ptr != curr_dec) {
        needsRedraw();
      }
    }

    void setPercentage(const float perc) {
      p = perc;
      value = minv +((maxv-minv) * p);
    }

    void setValue(const T& v) {
      value = std::max<T>(minv, std::min<T>(maxv, v));
      T nrange = maxv-minv;
      p = (float)1.0f/nrange * (value-minv);
    }
	
    Slider& setStep(const T s) {
      stepv = s;
      return *this;
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

    void save(config_setting_t* setting) {
      if(value_type == SLIDER_INT) {
        config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_INT);
        config_setting_set_int(cfg_el, value);
      }
      else if(value_type == SLIDER_FLOAT) {
        config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_FLOAT);
        config_setting_set_float(cfg_el, value);
      }
    }
	
    void load(config_setting_t* setting) {
      std::string cname = buttons_create_clean_name(name);
      if(value_type == SLIDER_INT) {
        int r = config_setting_lookup_int((const config_setting_t*)setting, cname.c_str(), (int*)&value);
        if(r == CONFIG_FALSE) {
          printf("ERROR: wront setting for: %s\n", cname.c_str());
        }
        else {
          setValue(value);
        }
      }
      else if(value_type == SLIDER_FLOAT) {
        double tmp_value = 0.0; 
        int r = config_setting_lookup_float((const config_setting_t*)setting, cname.c_str(), &tmp_value);
        if(r == CONFIG_FALSE) {
          printf("ERROR: wront setting for: %s\n", cname.c_str());
        }
        else {
          setValue(tmp_value);
        }
      }

      needsRedraw();
      needsTextUpdate();
    }
	
    Slider<T>& hide() {
      this->is_visible = false;
      static_text->setTextVisible(label_dx, false);
      dynamic_text->setTextVisible(txtval_dx, false);
      return *this;
    }
	
    Slider<T>& show() {
      this->is_visible = true;
      static_text->setTextVisible(label_dx, true);
      dynamic_text->setTextVisible(txtval_dx, true);
      updateTextPosition();
      return *this;
    }

    bool serializeScheme(ButtonsBuffer& buffer) {
      buffer.addByte(value_type);
      if(value_type == SLIDER_FLOAT) {
        buffer.addFloat(value);
        buffer.addFloat(minv);
        buffer.addFloat(maxv);
        buffer.addFloat(stepv);
      }
      else {
        buffer.addI32(value);
        buffer.addI32(minv);
        buffer.addI32(maxv);
        buffer.addI32(stepv);
      }
      return true;
    }
		

    T& value;
    T minv; // min value
    T maxv; // max value 
    T stepv; // step value
    float p; // current percentage
    int value_type; // SLIDER_FLOAT, SLIDER_INT (for storage)
	
    float bar_filled_color[4];
    float bar_filled_bottom[4];
    float bar_empty_color[4];
    int txtval_dx;
    int label_dx;

    // increment/decrement buttons
    int bt_mouse_down; // 0 = inc, 1 = decr, -1 not in button
    float bt_dec_x;
    float bt_inc_x;
    float bt_y;
    float bt_w;
    float bt_h; 
    float bt_top_col[4];
    float bt_bottom_col[4];
    float* bt_inc_top_color_ptr;
    float* bt_inc_bottom_color_ptr;
    float* bt_dec_top_color_ptr;
    float* bt_dec_bottom_color_ptr;
  };

  typedef Slider<float> Sliderf;
  typedef Slider<int> Slideri;

} // namespace buttons

#endif
