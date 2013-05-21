#ifndef ROXLU_BCOLORH
#define ROXLU_BCOLORH

#include <roxlu/core/Constants.h>
#include <buttons/Types.h>
#include <buttons/Slider.h>
#include <buttons/Element.h>

namespace buttons {

  struct ColorPickerVertex {
  ColorPickerVertex():H(0.0f){}
    ButtonVertex bv;
    float H; // the hue
  };

  struct ColorPickerCoords {
  ColorPickerCoords()
  :circle_cx(0.0f)
  ,circle_cy(0.0f)
      ,circle_radius(75.0f)
      ,circle_radius_sq(circle_radius * circle_radius)
      ,preview_x(0)
      ,preview_y(0)
      ,preview_w(0)
      ,preview_h(0)
      ,selected_color_radius(10.0f)
      ,selected_color_angle(0.0f)
      ,selected_color_dist(0.0f)
    {
    }
	
    int preview_x;
    int preview_y;
    int preview_w;
    int preview_h;
    float circle_cx;
    float circle_cy;
    float circle_radius;
    float circle_radius_sq;
    float selected_color_radius;
    float selected_color_angle;
    float selected_color_dist;
	
  };

  // T = color container, we assume RGBA colors, that can be accessed by [0]..[3]
  template<class T>
    class ColorT : public Element{
  public:	
    ColorT(const string& name, T* color);
    ~ColorT();
	
    void generateStaticText();
    void generateDynamicText();
    void generateVertices(ButtonVertices& shapeVertices);
    void updateTextPosition();
    void updateDynamicText();
	
    void getChildElements(vector<Element*>& elements);
    void positionChildren();
    void onChildValueChanged();

    void onMouseDown(int mx, int my);
    void onMouseUp(int mx, int my);
    void onMouseEnter(int mx, int my);
    void onMouseLeave(int mx, int my);	
    void onMouseClick(int mx, int my);
    void onMouseMoved(int mx, int my); // global move, using is_mouse_inside
    void onMouseDragged(int mx, int my, int dx, int dy);
	
    void save(std::ofstream& ofs);
    void load(std::ifstream& ifs);
    void onLoaded();
    bool canSave();
    bool serializeScheme(ButtonsBuffer& buffer);
	
    ColorT<T>&  hide();
    ColorT<T>&  show();

    void setValue(void* data); // used by e.g. client-server 

    int label_dx;	
    T* color;
	
    int H; // 0 - 360
    int S; // 0 - 100
    int L; // 0 - 100
    int A; // 0 - 100, alpha
    int h_big; 
    int h_small; 
    bool is_big;
    bool drag_started_in_circle;
    Slideri hue_slider;
    Slideri sat_slider;
    Slideri light_slider;
    Slideri alpha_slider; 
    ColorPickerCoords coords;
    vector<ColorPickerVertex> circle_vertices; 
    int event_data_values[4]; // used to send the color values as "event_data"
  };

  template<class T>
    ColorT<T>::ColorT(const string& name, T* color)
    :Element(BTYPE_COLOR, name)
    ,color(color)
    ,label_dx(-1)
    ,H(0)
    ,S(1)
    ,L(60)
    ,A(1)
    ,is_big(false)
    ,drag_started_in_circle(false)
    ,h_big(140)
    ,h_small(20)
    ,hue_slider(H, name+"_H", Slideri::SLIDER_INT)
    ,sat_slider(S, name+"_S", Slideri::SLIDER_INT)
    ,light_slider(L, name+"_L", Slideri::SLIDER_INT)
    ,alpha_slider(A, name+"_A", Slideri::SLIDER_INT)

    {
      this->h = h_small;
      hue_slider.label = "H";
      sat_slider.label = "S";
      light_slider.label = "L";
      alpha_slider.label = "A";
      hue_slider.setMin(0).setMax(360);
      sat_slider.setMin(0).setMax(100);
      light_slider.setMin(0).setMax(100);
      alpha_slider.setMin(0).setMax(100);
      event_data_values[0] = H;
      event_data_values[1] = S;
      event_data_values[2] = L;
      event_data_values[3] = A;
      event_data = (void*)event_data_values;
    }


  template<class T>
    ColorT<T>::~ColorT(){
  }


  template<class T>
    void ColorT<T>::getChildElements(vector<Element*>& elements) {
    elements.push_back(&hue_slider);
    elements.push_back(&sat_slider);
    elements.push_back(&light_slider);
    elements.push_back(&alpha_slider);
  }

  template<class T>
    void ColorT<T>::positionChildren() {
    float p_circle = 0.7;
    float p_sliders = 0.4;
    float circle_space = (this->w > this->h) ? this->h : this->w;
	
    coords.circle_radius = circle_space * (p_circle * 0.5);
    coords.circle_radius_sq = coords.circle_radius * coords.circle_radius;
    coords.circle_cx = 10 + this->x + coords.circle_radius;
    coords.circle_cy = this->y + coords.circle_radius + 0.5 * (this->h+20 - (coords.circle_radius*2));
    coords.selected_color_radius = 3.0f;

    hue_slider.x = coords.circle_cx + coords.circle_radius + 15;
    hue_slider.y = coords.circle_cy - (2.0 * hue_slider.h);
    hue_slider.w = this->w * p_sliders;
	
    sat_slider.x = hue_slider.x;
    sat_slider.y = hue_slider.y + hue_slider.h;
    sat_slider.w = hue_slider.w;

    light_slider.x = sat_slider.x;
    light_slider.y = sat_slider.y + sat_slider.h;
    light_slider.w = sat_slider.w;
	
    alpha_slider.x = light_slider.x;
    alpha_slider.y = light_slider.y + light_slider.h;
    alpha_slider.w = light_slider.w;

    coords.preview_w = 20;
    coords.preview_h = 15;
    coords.preview_x = (this->x + this->w) - (coords.preview_w + 5);
    coords.preview_y = this->y + 3; 
  }

  template<class T> 
    void ColorT<T>::onChildValueChanged() {
    event_data_values[0] = hue_slider.value;
    event_data_values[1] = sat_slider.value;
    event_data_values[2] = light_slider.value;
    event_data_values[3] = alpha_slider.value;
    //printf("ColorT, changed value of hue:%u\n", hue_slider.value);
  }

  template<class T>
    void ColorT<T>::generateStaticText() {
    this->label_dx = static_text->add(this->x+4, this->y+2, this->label,  0.9, 0.9, 0.9, 0.9);
  }

  template<class T>
    void ColorT<T>::generateDynamicText() {
  }

  template<class T>
    void ColorT<T>::updateTextPosition() {
    static_text->setTextPosition(this->label_dx, this->x+4, this->y+2);
  }

  template<class T>
    void ColorT<T>::updateDynamicText() {
  }

  template<class T>
    void ColorT<T>::generateVertices(ButtonVertices& vd) {

    // CREATE SMALL VERSION OF COLOR PICKER
    // ++++++++++++++++++++++++++++++++++++++++
    if(light_slider.is_visible && !is_big) {
      light_slider.hide();
      sat_slider.hide();
      hue_slider.hide();
      alpha_slider.hide();
    }
    else if(!light_slider.is_visible && is_big) {
      light_slider.show();
      sat_slider.show();
      hue_slider.show();
      alpha_slider.show();
    }
	
    // create background
    buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color);
	
    // Calculate the alpha background vertices (the transparency grey/white blocks)
    float block_col_white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float block_col_dark[4] = {0.4f, 0.4f, 0.4f, 1.0f};
    float* block_col = block_col_white;
    int num_blocks_i = 5; // must be odd
    int num_blocks_j = 3; // must be odd
    int block_w = float(coords.preview_w) / num_blocks_i;
    int block_h = float(coords.preview_h) / num_blocks_j;
    for(int i = 0; i < num_blocks_i; ++i) {
      for(int j = 0; j < num_blocks_j; ++j) {
        int dx = (j * num_blocks_i) + i;
        if(dx%2 == 0) {
          block_col = block_col_white;
        }
        else {
          block_col = block_col_dark;
        }
			
        int bv_x = coords.preview_x + i * block_w;
        int bv_y = coords.preview_y + j * block_h;
        buttons::createRect(vd, bv_x, bv_y, block_w, block_h, block_col, block_col);
      }
    }
			
    //hue_slider.generateVertices(vd);
    color[3] = float(A)/100.0f;
    HSL_to_RGB(float(hue_slider.value)/360.0f, float(sat_slider.value)/100.0f, float(light_slider.value)/100.0f, &color[0], &color[1], &color[2]);

    // create color rect
    buttons::createRect(vd, coords.preview_x, coords.preview_y, coords.preview_w, coords.preview_h, color, color);
	
    if(!is_big) {
      return;
    }
	
    // CREATE EXPANDED VERSION OF COLOR PICKER
    // ++++++++++++++++++++++++++++++++++++++++
	
    // Execute once: calculate the vertices
    if(!circle_vertices.size()) {
      int res = 36;			
      circle_vertices.assign(res+1, ColorPickerVertex());
      float ap = 6.28318531 / res;
      for(int i = 0; i <= res; ++i) {
        float a = i * ap;
        circle_vertices[i].bv.setPos(cos(a) * coords.circle_radius,sin(a) * coords.circle_radius);
        circle_vertices[i].H = (float(i)/res);
      }
    }


    // using the current L value we change the colors of the circle
    float outline_col[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float hsl_col[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    vector<ButtonVertex> ui_circle_fill;
    vector<ButtonVertex> ui_circle_outline;
    ui_circle_fill.push_back(ButtonVertex(coords.circle_cx, coords.circle_cy, hsl_col));
	
    for(std::vector<ColorPickerVertex>::iterator it = circle_vertices.begin(); it != circle_vertices.end(); ++it) {
      ColorPickerVertex& pv = *it;
      //float(S)/100.0f
      HSL_to_RGB(pv.H, 1.0, 0.4, &hsl_col[0], &hsl_col[1], &hsl_col[2]);
      ButtonVertex bv(coords.circle_cx + pv.bv.pos[0], coords.circle_cy + pv.bv.pos[1], hsl_col);
      ui_circle_fill.push_back(bv);
      bv.setCol(outline_col);
      ui_circle_outline.push_back(bv);
    }
    vd.add(ui_circle_fill, GL_TRIANGLE_FAN);
    vd.add(ui_circle_outline, GL_LINE_STRIP);
	
    // The selected color point.
    float sel_col_black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float sel_col_white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float sel_angle = (float(H)) * DEG_TO_RAD;
    float sel_dist = (float(S)/100.0f) * coords.circle_radius;
    float sel_col_x = coords.circle_cx + cos(sel_angle) * sel_dist;
    float sel_col_y = coords.circle_cy + sin(sel_angle) * sel_dist;

    buttons::createCircle(vd, sel_col_x, sel_col_y, coords.selected_color_radius+1, sel_col_black);
    buttons::createCircle(vd, sel_col_x, sel_col_y, coords.selected_color_radius, sel_col_white);
  }


  template<class T>
    void ColorT<T>::onMouseDown(int mx, int my) {
    drag_started_in_circle = false;
    if(is_mouse_inside && is_big) {	
      // check if the draw started inside the circle
      float dist_x = coords.circle_cx - mx;
      float dist_y = coords.circle_cy - my;
      float dist_sq = (dist_x * dist_x) + (dist_y * dist_y);
      drag_started_in_circle = dist_sq < coords.circle_radius_sq;
    }
  }

  template<class T>
    void ColorT<T>::onMouseUp(int mx, int my) {
    drag_started_in_circle = false;
  }

  template<class T>
    void ColorT<T>::onMouseEnter(int mx, int my) {
    this->bg_top_color = this->col_bg_top_hover;
    this->bg_bottom_color = this->col_bg_bottom_hover;
    needsRedraw();
  }

  template<class T>
    void ColorT<T>::onMouseLeave(int mx, int my) {
    this->bg_top_color = this->col_bg_default;
    this->bg_bottom_color = this->col_bg_default;
    needsRedraw();
  }

  template<class T>
    void ColorT<T>::onMouseClick(int mx, int my) {
    if(is_mouse_inside) {
      if(BMOUSE_INSIDE(mx, my, coords.preview_x, coords.preview_y, coords.preview_w, coords.preview_h)) {
        is_big = !is_big;
        this->h = (is_big) ? h_big : h_small;
        positionChildren();
        needsRedraw();
      }
    }
  }

  template<class T>
    void ColorT<T>::onMouseMoved(int mx, int my) {
  }

  template<class T>
    void ColorT<T>::onMouseDragged(int mx, int my, int dx, int dy) {

    if(drag_started_in_circle) {	
      float dist_x = coords.circle_cx - mx;
      float dist_y = coords.circle_cy - my;
      float dist_sq = (dist_x * dist_x) + (dist_y * dist_y);
      if(dist_sq > coords.circle_radius_sq) {
        dist_sq = coords.circle_radius_sq;
      }

      coords.selected_color_dist = sqrt(dist_sq);
      coords.selected_color_angle = atan2(-dist_y, -dist_x);
	
      if(coords.selected_color_angle < 0) {
        H = (coords.selected_color_angle + TWO_PI) * RAD_TO_DEG;
      }
      else {
        H = coords.selected_color_angle * RAD_TO_DEG;
      }
      S = (coords.selected_color_dist / coords.circle_radius) * 100;
      hue_slider.setValue(H);
      sat_slider.setValue(S);
      sat_slider.needsRedraw();
      hue_slider.needsRedraw();
      sat_slider.needsTextUpdate();
      hue_slider.needsTextUpdate();

      //printf("Storing: %u %u %u %u\n", H, S, L, A);
      event_data_values[0] = H;
      event_data_values[1] = S;
      event_data_values[2] = L;
      event_data_values[3] = A;
      flagValueChanged(); 
    }	
  }

  template<class T>
    bool ColorT<T>::canSave() {
    return false;
  }

  template<class T>
    void ColorT<T>::save(std::ofstream& ofs) {
  }

  template<class T>
    void ColorT<T>::load(std::ifstream& ifs) {
  }

  template<class T>
    void ColorT<T>::onLoaded() {
    color[3] = float(A)/100.0f;
    HSL_to_RGB(float(hue_slider.value)/360.0f, float(sat_slider.value)/100.0f, float(light_slider.value)/100.0f, &color[0], &color[1], &color[2]);
  } 

  template<class T>
    ColorT<T>& ColorT<T>::hide() {
    this->is_visible = false;
    this->static_text->setTextVisible(this->label_dx, false);
    return *this;
  }

  template<class T>
    ColorT<T>& ColorT<T>::show() {
    this->is_visible = true;
    this->static_text->setTextVisible(this->label_dx, true);
    return *this;
  }

  template<class T>
    void ColorT<T>::setValue(void* hsla) {
    unsigned int* col_ptr = (unsigned int*)hsla;
    hue_slider.setValue(col_ptr[0]);
    sat_slider.setValue(col_ptr[1]);
    light_slider.setValue(col_ptr[2]);
    alpha_slider.setValue(col_ptr[3]);
    sat_slider.needsRedraw();
    hue_slider.needsRedraw();
    needsRedraw();
  }

  
  template<class T>
    bool ColorT<T>::serializeScheme(ButtonsBuffer& buffer) {
    buffer.addUI32(hue_slider.value);
    buffer.addUI32(sat_slider.value);
    buffer.addUI32(light_slider.value);
    buffer.addUI32(alpha_slider.value);
    return true;
  }

  typedef ColorT<float> ColorPicker;

} // buttons

#endif
