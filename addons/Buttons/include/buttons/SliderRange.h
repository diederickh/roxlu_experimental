#ifndef ROXLU_BSLIDER_RANGE_H
#define ROXLU_BSLIDER_RANGE_H

#include <sstream>                                                  /* used to convert the value to string */
#include <buttons/Element.h>

namespace buttons {


  // ---------------------------------------------------------

  template<class T>
    class SliderRange : public Element {
  public:
    SliderRange(T* val, const string& name);

    // drawing
    void generateVertices(ButtonVertices& shapeVertices);
    SliderRange<T>& hide();
    SliderRange<T>& show();

    // interaction
    void onMouseMoved(int mx, int my);
    void onMouseDragged(int mx, int my, int dx, int dy);
    void onMouseEnter(int mx, int my);
    void onMouseLeave(int mx, int my);
    void onMouseUp(int mx, int my);
    void onMouseDown(int mx, int my);

    // storage & data
    void updateValue();                                             /* updates the current min/max value using min_p and max_p */
    void setMinValue(T v);                                          /* set the value for the minimum entry e.g. 10.0f */
    void setMaxValue(T v);                                          /* set the value for the maximum entry e.g. 1000.0f */
    bool canSave();
    void save(config_setting_t* setting);                           /* save the values to the settings file */
    void save(config_setting_t* setting, float* v);                 /* save for T = float */
    void save(config_setting_t* setting, int* v);                   /* save for T = int */
    void load(config_setting_t* setting);                           /* load the values from the settings */
    void load(config_setting_t* setting, float* v);                 /* load float T = float */
    void load(config_setting_t* setting, int* v);                   /* load float T = int */

    void save(std::ofstream& ofs);                                   /* @deprecated */
    void load(std::ifstream& ifs);                                   /* @deprecated */
    SliderRange<T>& setMin(T val);
    SliderRange<T>& setMax(T val);
    SliderRange<T>& setColor(float hue, float sat, float bright, float a = 1.0f);
    SliderRange<T>& setStep(T step);                     

    // text
   void generateStaticText();
   void updateTextPosition();
   void generateDynamicText();
   void updateDynamicText();

  public:
   // storage and data
   T min_val;                                                        /* the minimum value for this range */
   T max_val;                                                        /* the maximum value for this range */
   T* value;                                                         /* array of T, 0 = min value, 1 = max value */
   T step_val;                                                       /* used to increment/decrement the last selected handle */
   int label_dx;                                                     /* the index of the static text */
   int value_dx;                                                     /* text index for value */
   float min_p;                                                      /* the percentage of the minimum handle */
   float max_p;                                                      /* hte percentage of the maximum handle */

   // interaction
   int active_bt;                                                    /* which button is active: -1 none, 0 = left handle, 1 = right handle, 2 = decrement, 3 = increment */
   int last_active_handle;                                           /* which handle was selected, -1 = none, 1 = left, 2 = right */

   // positioning
   int bar_w;                                                        /* the maximum width of the bar. from left most X to left most x of the buttons */
   int bar_h;                                                        /* the height of the bar which indicates the current value */
   int handle_w;                                                     /* the handle width that allows the user to drag the min and max values */
   int handle_y;                                                     /* top y-position for the handles */
   int min_handle_x;                                                 /* left most x-position of the handle for the minimum value */
   int max_handle_x;                                                 /* left most x-position of the handle for the maximum value */
   int bt_w;                                                         /* the width of one increment or decrement button */
   int bt_dec_x;                                                     /* x position of the decrement button */
   int bt_inc_x;                                                     /* x position of the increment button */ 
   int bt_y;                                                         /* y of the buttons */
   int bt_h;                                                         /* height of the buttons */

   // colors
   float handle_default_color[4];                                    /* the default color of a handle */
   float handle_hover_color[4];                                      /* the color of a handle when the user mouse overs a handle */
   float handle_indicator_default_color[4];                          /* default color for the direction indicator */
   float handle_indicator_active_color[4];                           /* color for the selected handle indicator; when you press the increment/decrement buttons this handle will change */
   float bar_filled_color[4];                                        /* the color of the bar which is drawn between the handles */
   float bar_filled_bottom[4];                                       /* the color of the bar which is drawn between the handles, this is the "bottom" color of the bar creating a gradient  */
   float bar_empty_color[4];                                         /* the color of the empty parts of the bar */
   float bt_top_color[4];                                              /* top color for the increment / decrement buttons */
   float bt_bottom_color[4];                                           /* bottom color for the incremetn / decrement buttons */

   float* handle_left_color;                                         /* current color for the left handle */
   float* handle_right_color;                                        /* current color for the right handle */
   float* handle_left_indicator_color;                               /* color for the left indicator */
   float* handle_right_indicator_color;                              /* color for the right indicator */
   float* bt_dec_top_color;                                          /* color of the decrement button: top */
   float* bt_dec_bottom_color;                                       /* color of the decrement button: bottom */
   float* bt_inc_top_color;                                          /* color of the increment button: top */
   float* bt_inc_bottom_color;                                       /* color of the increment button: bottom */
  };

  // ---------------------------------------------------------

  template<class T>
    SliderRange<T>::SliderRange(T* val, const string& name)
    :Element(BTYPE_SLIDER_RANGE, name)
    ,label_dx(-1)
    ,min_val(0)
    ,max_val(0)
    ,min_handle_x(0)
    ,max_handle_x(0)
    ,handle_w(10)
    ,handle_y(0)
    ,bar_h(4)
    ,bt_w(20)
    ,active_bt(-1)
    ,min_p(0.0f)
    ,max_p(0.5f)
    ,value(val)
    ,last_active_handle(-1)
    ,bt_dec_x(0)
    ,bt_inc_x(0)
    ,bt_h(22)
    ,bt_y(0)
    ,step_val(0)
    {
      h = 22;
      bt_h = h;

      BSET_COLOR(handle_default_color, 242.0/255.0f, 232.0f/255.0f, 92.0f/255.0f, 1.0f);
      BSET_COLOR(handle_hover_color, 4.0/255.0f, 191.0f/255.0f, 157.0f/255.0f, 1.0f);
      BSET_COLOR(handle_indicator_default_color, 242.0/255.0f, 53.0f/255.0f, 0.0f, 1.0f);
      BSET_COLOR(handle_indicator_active_color, 107.0/255.0f, 166.0f/255.0f, 217.0/255.0f, 1.0f);

      handle_left_color = handle_default_color;
      handle_right_color = handle_default_color;
      handle_left_indicator_color = handle_indicator_default_color;
      handle_right_indicator_color = handle_indicator_default_color;
      bt_dec_top_color = bt_top_color;
      bt_inc_top_color = bt_top_color;
      bt_dec_bottom_color = bt_bottom_color;
      bt_inc_bottom_color = bt_bottom_color;
    }

  // DRAWING
  // ---------------------------------------------------------
  template<class T>
    void SliderRange<T>::generateVertices(ButtonVertices& vd) {
    
    buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color); // background

    // handle positions;
    bar_w = w - (2 * bt_w);
    handle_y = y + (h - bar_h);

    max_handle_x = x + max_p * (bar_w - handle_w);
    min_handle_x = x + min_p * (bar_w - handle_w);

    bt_dec_x = x + bar_w;
    bt_inc_x = bt_dec_x + bt_w;
    bt_y = y;
    bt_h = h;

    // filled + unfilled bar
    buttons::createRect(vd, x, handle_y, bar_w, bar_h, bar_empty_color, bar_empty_color); // unfilled color
    buttons::createRect(vd, min_handle_x, handle_y, (max_handle_x - min_handle_x), bar_h, bar_filled_color, bar_filled_bottom); // unfilled color
    
    // left & right handles
    buttons::createRect(vd, min_handle_x, handle_y, handle_w, bar_h, handle_left_color, handle_left_color); // left
    buttons::createRect(vd, max_handle_x, handle_y, handle_w, bar_h, handle_right_color, handle_right_color); // right
    buttons::createRect(vd, min_handle_x, handle_y, 2, bar_h, handle_left_indicator_color, handle_left_indicator_color); // left
    buttons::createRect(vd, max_handle_x + handle_w - 2, handle_y, 2, bar_h, handle_right_indicator_color, handle_right_indicator_color); // right

    // buttons
    buttons::createRect(vd, bt_dec_x, bt_y, bt_w, bt_h, bt_dec_top_color, bt_dec_bottom_color); // dec bg
    buttons::createRect(vd, bt_inc_x, bt_y, bt_w, bt_h, bt_inc_top_color, bt_inc_bottom_color); // inc bg
    buttons::createRect(vd, bt_inc_x + bt_w * 0.2, bt_y + bt_h * 0.45, bt_w * 0.6, 2, col_text, col_text); // + sign
    buttons::createRect(vd, bt_inc_x + bt_w * 0.45, bt_y + bt_h * 0.20, 2, bt_h * 0.6, col_text, col_text); // + sign		
    buttons::createRect(vd, bt_dec_x + bt_w * 0.2, bt_y + bt_h * 0.45, bt_w * 0.6, 2, col_text, col_text); // - sign
  }

  template<class T>
    SliderRange<T>& SliderRange<T>::hide() {
    this->is_visible = false;
    static_text->setTextVisible(label_dx, false);
    dynamic_text->setTextVisible(value_dx, false);
    return *this;
  }

	template<class T>
    SliderRange<T>& SliderRange<T>::show() {
    this->is_visible = true;
    static_text->setTextVisible(label_dx, true);
    dynamic_text->setTextVisible(value_dx, true);
    updateTextPosition();
    return *this;
  }

  // STORAGE AND DATA
  // ---------------------------------------------------------
  template<class T>
    bool SliderRange<T>::canSave() {
    return true;
  }

  template<class T>
    void SliderRange<T>::save(config_setting_t* setting) {
    save(setting, value);
  }

  template<class T>
    void SliderRange<T>::save(config_setting_t* setting, float* v) {
    config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
    config_setting_set_float_elem(cfg_el, -1, v[0]);
    config_setting_set_float_elem(cfg_el, -1, v[1]);
  }

  template<class T>
    void SliderRange<T>::save(config_setting_t* setting, int* v) {
    config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_ARRAY);
    config_setting_set_int_elem(cfg_el, -1, v[0]);
    config_setting_set_int_elem(cfg_el, -1, v[1]);
  }

  template<class T>
    void SliderRange<T>::load(config_setting_t* setting) {

    std::string cname = buttons_create_clean_name(name);

    config_setting_t* cfg = config_setting_get_member(setting, cname.c_str());
    if(!cfg) {
      RX_ERROR("Cannot find SliderRange setting.\n");
      return;
    }

    load(cfg, value);

    setMinValue(value[0]);
    setMaxValue(value[1]);
    needsRedraw();
  }

  template<class T>
    void SliderRange<T>::load(config_setting_t* cfg, float* v) {
    v[0] = config_setting_get_float_elem(cfg, 0);
    v[1] = config_setting_get_float_elem(cfg, 1);
  }

  template<class T>
    void SliderRange<T>::load(config_setting_t* cfg, int* v) {
    v[0] = config_setting_get_int_elem(cfg, 0);
    v[1] = config_setting_get_int_elem(cfg, 1);
  }
    
  template<class T>
    SliderRange<T>& SliderRange<T>::setMin(T val) {
    min_val = val;
    return *this;
  }

  template<class T>
    SliderRange<T>& SliderRange<T>::setMax(T val) {
    max_val = val;
    return *this;
  }

  template<class T>
    void SliderRange<T>::updateValue() {
    T range = max_val - min_val;
    value[0] = min_val + min_p * range;
    value[1] = min_val + max_p * range;
  }

  template<class T>
    SliderRange<T>& SliderRange<T>::setStep(T step) {
    step_val = step;
    return *this;
  }

  template<class T>
    void SliderRange<T>::setMinValue(T v) {
    value[0] = std::max<T>(min_val, std::min<T>(max_val, v));
    T range = max_val - min_val;
    min_p  = (float)1.0f/range * (value[0] - min_val);
  }

  template<class T>
    void SliderRange<T>::setMaxValue(T v) {
    value[1] = std::max<T>(min_val, std::min<T>(max_val, v));
    T range = max_val - min_val;
    max_p  = (float)1.0f/range * (value[1] - min_val);
  }

  template<class T>
    SliderRange<T>& SliderRange<T>::setColor(float hue, float sat, float bright, float a) {
    Element::setColor(hue, sat, bright, a);

    bar_empty_color[3] = a;
    bar_filled_color[3] = a;
    bar_filled_bottom[3] = a;
    handle_default_color[3] = a;
    handle_hover_color[3] = a;
    bt_top_color[3] = a;
    bt_bottom_color[3] = a;
    
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, &bar_empty_color[0], &bar_empty_color[1], &bar_empty_color[2]);
    HSL_to_RGB(col_hue, col_sat, col_bright + 0.4, &bar_filled_color[0], &bar_filled_color[1], &bar_filled_color[2]);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, &bar_filled_bottom[0], &bar_filled_bottom[1], &bar_filled_bottom[2]);
    
    HSL_to_RGB(col_hue, col_sat - 0.1, col_bright, &handle_default_color[0], &handle_default_color[1], &handle_default_color[2]);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, &handle_hover_color[0], &handle_hover_color[1], &handle_hover_color[2]);
    
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1, bt_top_color, bt_top_color+1, bt_top_color+2);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.2, bt_bottom_color, bt_bottom_color+1, bt_bottom_color+2);
    return *this;
  }

  // INTERACTION
  // ---------------------------------------------------------

  template<class T>
    void SliderRange<T>::onMouseMoved(int mx, int my) {
    float* curr_handle_left_color = handle_left_color;
    float* curr_handle_right_color = handle_right_color;
    float* curr_inc_top_color = bt_inc_top_color;
    float* curr_dec_top_color = bt_dec_top_color;

    // check left handle
    if(BMOUSE_INSIDE(mx, my, min_handle_x, y, handle_w, h)) {
      handle_left_color = handle_hover_color;
    }
    else {
      handle_left_color = handle_default_color;
    }

    // check right handle
    if(BMOUSE_INSIDE(mx, my, max_handle_x, y, handle_w, h)) {
      handle_right_color = handle_hover_color;
    }
    else {
      handle_right_color = handle_default_color;
    }

    // check increment button
    if(BMOUSE_INSIDE(mx,my,bt_inc_x, bt_y, bt_w, bt_h)) {
      bt_inc_top_color = bt_bottom_color;
      bt_inc_bottom_color = bt_top_color;
    }
    else {
      bt_inc_top_color = bt_top_color;
      bt_inc_bottom_color = bt_bottom_color;
    }

    // check decrement button
    if(BMOUSE_INSIDE(mx,my,bt_dec_x, bt_y, bt_w, bt_h)) {
      bt_dec_top_color = bt_bottom_color;
      bt_dec_bottom_color = bt_top_color;
    }
    else {
      bt_dec_top_color = bt_top_color;
      bt_dec_bottom_color = bt_bottom_color;
    }

    // did something change?
    if(handle_left_color != curr_handle_left_color
       || handle_right_color != curr_handle_right_color
       || bt_inc_top_color != curr_inc_top_color
       || bt_dec_top_color != curr_dec_top_color
    )
      {
      needsRedraw();
    }
  }

  template<class T>
    void SliderRange<T>::onMouseDragged(int mx, int my, int dx, int dy) {

    if(drag_inside) {
      if(active_bt == 0) {
        int range_w = bar_w - handle_w;
        int local_x = std::min<int>(x + range_w, std::max<int>(x, mx));
        min_p = float(local_x - x) / range_w;
      }
      else if(active_bt == 1) {
        int range_w = bar_w - handle_w;
        int local_x = std::min<int>(x + range_w, std::max<int>(x, mx));
        max_p = float(local_x - x) / range_w;
      }

      updateValue();
      needsRedraw();
      needsTextUpdate();
    }
  }

  template<class T>
    void SliderRange<T>::onMouseEnter(int mx, int my) {
  }

  template<class T>
    void SliderRange<T>::onMouseLeave(int mx, int my) {
  }

  template<class T>
    void SliderRange<T>::onMouseUp(int mx, int my) {

    if(is_mouse_inside) {
      // increment button
      if(active_bt == 2 && BMOUSE_INSIDE(mx,my,bt_inc_x, bt_y, bt_w, bt_h)) {
        if(last_active_handle == 0) {
          setMinValue(value[0] + step_val);
          needsRedraw();
        }
        else if(last_active_handle == 1) {
          setMaxValue(value[1] + step_val);
          needsRedraw();
        }
      }
      // decrement button
      else if(active_bt == 3 && BMOUSE_INSIDE(mx,my,bt_dec_x, bt_y, bt_w, bt_h)) {
        if(last_active_handle == 0) {
          setMinValue(value[0] - step_val);
          needsRedraw();
        }
        else if(last_active_handle == 1) {
          setMaxValue(value[1] - step_val);
          needsRedraw();
        }
      }
    }
  }

  template<class T>
    void SliderRange<T>::onMouseDown(int mx, int my) {

    // left handle
    if(BMOUSE_INSIDE(mx, my, min_handle_x, y, handle_w, h)) {
      active_bt = 0;
      last_active_handle = 0;
      handle_left_indicator_color = handle_indicator_active_color;
      handle_right_indicator_color = handle_indicator_default_color;
    }
    // right handle
    else if(BMOUSE_INSIDE(mx, my, max_handle_x, y, handle_w, h)) {
      active_bt = 1;
      last_active_handle = 1;
      handle_left_indicator_color = handle_indicator_default_color;
      handle_right_indicator_color = handle_indicator_active_color;
    }
    // check increment button
    else if(BMOUSE_INSIDE(mx,my,bt_inc_x, bt_y, bt_w, bt_h)) {
      active_bt = 2;
    }
    // check decrement button
    else if(BMOUSE_INSIDE(mx,my,bt_dec_x, bt_y, bt_w, bt_h)) {
     active_bt = 3;
    }
    else {
      active_bt = -1;
    }
  }


  // TEXT
  // ---------------------------------------------------------
  template<class T>
    void SliderRange<T>::generateStaticText() {
    label_dx = static_text->add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
  }
		
  template<class T>
    void SliderRange<T>::updateTextPosition() {
    static_text->setTextPosition(label_dx, x+4, y+2);
    updateDynamicText();
  }
	
  template<class T>
    void SliderRange<T>::generateDynamicText() {
    value_dx = dynamic_text->add(x,y+2, "0.0 / 0.0", col_text[0], col_text[1], col_text[2], col_text[3]);
    dynamic_text->setTextAlign(value_dx, TEXT_ALIGN_RIGHT, (w - 50));
  }
	
  template<class T>
    void SliderRange<T>::updateDynamicText() {
    std::stringstream ss;
    ss << value[0] << " / " << value[1];
    dynamic_text->updateText(value_dx, ss.str().c_str(), col_text[0], col_text[1], col_text[2], col_text[3]);
    dynamic_text->setTextPosition(value_dx, x, y+2);		
  }

  // @deprecated!
  template<class T>
    void SliderRange<T>::save(std::ofstream& ofs) {
  }

  template<class T>
    void SliderRange<T>::load(std::ifstream& ofs) {
  }

  typedef SliderRange<float> SliderRangef;
  typedef SliderRange<int> SliderRangei;

} // namespace buttons
#endif
