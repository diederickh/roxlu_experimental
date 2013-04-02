#ifndef ROXLU_BSPLINEH
#define ROXLU_BSPLINEH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

  struct CurvePoint {
    float x,y;
  CurvePoint():x(0),y(0) {}
  CurvePoint(const float x, const float y):x(x),y(y) {}
  };

  // just a helper...
  struct EditorCoords {
  EditorCoords()
  :min_x(0)
  ,min_y(0)
      ,max_x(0)
      ,max_y(0)
      ,w(0)
      ,h(0)
      ,handle_s(5.0f)
      ,handle_hs(2.5)
      ,max_curve_w(0.0f)
      ,max_curve_h(0.0f)
    {
    }
	
    float min_x;
    float min_y;
    float max_x;
    float max_y;
    float w;
    float h;
    float handle_s;
    float handle_hs;
    float max_curve_h;
    float max_curve_w;
	
  };

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // It would probably be cleaner to let the "user" of this gui, define it's spline
  // ... but for now this will do.
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  // Experimental...
  // S: spline
  // V: vec type (vec2,vec3)
  template<class S, class V> 
    class SplineEditor : public Element {
  public:	
    SplineEditor(const string& name, S& spline);
    ~SplineEditor();

    void setup();
    void draw(Shader& shader, const float* pm);
	
    void generateStaticText();
    void updateTextPosition();
    void generateVertices(ButtonVertices& shapeVertices);
	
    void onMouseDown(int mx, int my);
    void onMouseUp(int mx, int my);
    void onMouseEnter(int mx, int my);
    void onMouseLeave(int mx, int my);	
    void onMouseClick(int mx, int my);
    void onMouseMoved(int mx, int my); // global move, using is_mouse_inside
    void onMouseDragged(int mx, int my, int dx, int dy);
	
    void save(std::ofstream& ofs);
    void load(std::ifstream& ifs);
    bool canSave();
	
    SplineEditor<S, V>& hide();
    SplineEditor<S, V>& show();
	
    SplineEditor<S, V>& setColor(const float hue, float sat = 0.2f, float bright = 0.27f, float a = 1.0f);
	
    int selected_handle_dx;
    float selected_handle_col[4];
    float* default_handle_col; // set to tex col
    float editor_bg_bottom_col[4];
    float editor_bg_top_col[4];
    float editor_spline_col[4];
	
    EditorCoords editor_coords;
    int label_dx;
    S& spline;
    vector<V> screen_handles;
  };


  template<class S, class V> 
    SplineEditor<S,V>::SplineEditor(const string& name, S& spline) 
    :Element(BTYPE_SPLINE, name)
    ,spline(spline)
    ,label_dx(0)
    ,selected_handle_dx(-1)
    {
      this->h = 75;	
      BSET_COLOR(selected_handle_col, 1.0, 0.7,0.0,1.0);
      default_handle_col = col_text;
    }

  template<class S, class V> 
    SplineEditor<S, V>& SplineEditor<S,V>::setColor(const float hue, float sat, float bright, float a) {
    Element::setColor(hue, sat, bright, a);
	
    // bar_empty_color: saturated default color
    editor_bg_top_col[3] = a;
    editor_spline_col[3] = a;
    default_handle_col[3] = a;

    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1,  &editor_bg_top_col[0], &editor_bg_top_col[1], &editor_bg_top_col[2]);
    BSET_COLOR(editor_bg_bottom_col, editor_bg_top_col[0], editor_bg_top_col[1], editor_bg_top_col[2], a);
    HSL_to_RGB(col_hue, col_sat, col_bright + 0.5,  &editor_spline_col[0], &editor_spline_col[1], &editor_spline_col[2]);
    return *this;
  }

  template<class S, class V>  
    SplineEditor<S,V>::~SplineEditor() {
  }

  template<class S, class V> 
    void SplineEditor<S,V>::generateStaticText() {
    this->label_dx = static_text->add(this->x+4, this->y+5, this->label,  0.9, 0.9, 0.9, 0.9);
  }

  template<class S, class V> 
    void SplineEditor<S,V>::updateTextPosition() {
    static_text->setTextPosition(this->label_dx, this->x+4, this->y+5);
  }

  template<class S, class V> 
    void SplineEditor<S,V>::generateVertices(ButtonVertices& vd) {
	
    buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color);

    // set sizes for all calculations
    float hh = h * 0.3333;
    editor_coords.min_x = this->x + 4;
    editor_coords.max_x = editor_coords.min_x + this->w - 10;
    editor_coords.min_y = hh+this->y;
    editor_coords.max_y = editor_coords.min_y + h - hh * 1.3;
    editor_coords.w = editor_coords.max_x - editor_coords.min_x;
    editor_coords.h = editor_coords.max_y - editor_coords.min_y;
    editor_coords.max_curve_h = editor_coords.h * 0.5;
	
    // Create a rectangle in which we draw the spline.
    buttons::createRect(vd, editor_coords.min_x, editor_coords.min_y, editor_coords.w, editor_coords.h, editor_bg_top_col, editor_bg_bottom_col);

    // calculate the handle coordinates
    screen_handles.clear();
    float max_hx = editor_coords.max_x-editor_coords.handle_hs-2; // max handle x
    for(int i = 0; i < spline.points.size(); ++i) {	
      V& p = spline.points[i];
      screen_handles.push_back(V(
                                   editor_coords.min_x + (p.x * editor_coords.w)
                                   ,editor_coords.max_y - (p.y * editor_coords.h)
                                 ));
    }

    // Create the line
    vector<ButtonVertex> spline_verts;
    int num = 50;
    V spline_pos;
    for(int i = 0; i <= num; ++i) {
      float p = float(i)/num;
      if(spline.at(p, spline_pos)) {
        spline_verts.push_back(ButtonVertex(
                                              editor_coords.min_x + spline_pos.x * editor_coords.w
                                              ,editor_coords.max_y - spline_pos.y * editor_coords.h
                                              ,editor_spline_col
                                            ));
      }
    }
    vd.add(spline_verts, GL_LINE_STRIP);

    // Create the handles.	
    float* handle_col = default_handle_col;
    for(int i = 0; i < screen_handles.size(); ++i) {
      V& p = screen_handles[i];
      handle_col = (i == selected_handle_dx) ? selected_handle_col : default_handle_col;
      buttons::createCircle(vd, p.x, p.y, editor_coords.handle_hs, handle_col);
    }
  }

  template<class S, class V> 
    void SplineEditor<S, V>::setup() {
    if(spline.size() == 0) {
      int num = 6;
      for(int i = 0; i < num; ++i) {
        float p = float(i)/(num-1);
        spline.add(V(p,0.5));
      }
    }
  }

  template<class S, class V> 
    void SplineEditor<S, V>::draw(Shader& shader, const float* pm) {
	
  }

  template<class S, class V> 
    void SplineEditor<S, V>::onMouseMoved(int mx, int my) {

    // find the handle which must be selected.
    float dist_sq = 225;
    float dx, dy, sq = 0.0f;
    int sel = 0;
    int curr_sel = selected_handle_dx;
    selected_handle_dx = -1;
    if(is_mouse_inside) {	
      for(typename vector<V>::iterator it = screen_handles.begin(); it != screen_handles.end(); ++it) {
        V& p = *it;
        dx = (mx - p.x);
        dy = (my - p.y);
        sq = (dx * dx) + (dy*dy);
			
        if(sq < dist_sq) {
          selected_handle_dx = sel;
        }
        ++sel;
      }
    }
    if(curr_sel != selected_handle_dx) {
      needsRedraw();		
    }
  }

  template<class S, class V> 
    void SplineEditor<S, V>::onMouseDragged(int mx, int my, int dx, int dy) {
    if(drag_inside && selected_handle_dx != -1) {
      V& screen_point = screen_handles[selected_handle_dx];
		
      // First, calculate the local relative position
      screen_point.x = float(mx) - editor_coords.handle_hs;
      screen_point.y = float(my) - editor_coords.handle_hs;
      float local_x = screen_point.x - editor_coords.min_x;
      float local_y = screen_point.y - editor_coords.min_y;
		
      // then convert this to the curve (which has a range betwee 0...1 for x and y)
      float xp = local_x / editor_coords.w;
      float yp =  1.0f - (local_y / editor_coords.h);
		
      spline.points[selected_handle_dx].x = std::max<float>(0.0f,std::min<float>(1.0f,xp));
      spline.points[selected_handle_dx].y = std::max<float>(0.0f,std::min<float>(1.0f,yp)); 
      needsRedraw();
    }
  }

  template<class S, class V> 
    void SplineEditor<S, V>::onMouseDown(int mx, int my) {
  }

  template<class S, class V> 
    void SplineEditor<S,V>::onMouseUp(int mx, int my) {
  }

  template<class S, class V> 
    void SplineEditor<S, V>::onMouseClick(int mx, int my) {
  }

  template<class S, class V> 
    void SplineEditor<S, V>::onMouseEnter(int mx, int my) {
    this->bg_top_color = this->col_bg_top_hover;
    this->bg_bottom_color = this->col_bg_bottom_hover;
    needsRedraw();
  }

  template<class S, class V> 
    void SplineEditor<S, V>::onMouseLeave(int mx, int my) {
    this->bg_top_color = this->col_bg_default;
    this->bg_bottom_color = this->col_bg_default;
    needsRedraw();
  }



  template<class S, class V> 
    bool SplineEditor<S, V>::canSave() {
    return true;
  }

  template<class S, class V> 
    void SplineEditor<S, V>::save(std::ofstream& ofs) {
    size_t data_size = sizeof(V) * spline.size() + sizeof(size_t); 
    ofs.write((char*)&data_size, sizeof(size_t));
	
    // number of elements.
    size_t num_els = spline.size();
    ofs.write((char*)&num_els, sizeof(size_t));
	
    // write elements.
    for(int i = 0; i < spline.size(); ++i) {
      V& p = spline[i];
      ofs.write((char*)&p.x, sizeof(float));
      ofs.write((char*)&p.y, sizeof(float));
    }
  }

  template<class S, class V> 
    void SplineEditor<S, V>::load(std::ifstream& ifs) {
    spline.clear();
    size_t num_saved = 0;
    ifs.read((char*)&num_saved, sizeof(size_t));
    for(int i = 0; i < num_saved; ++i) {
      V p;
      ifs.read((char*)&p.x, sizeof(float));
      ifs.read((char*)&p.y, sizeof(float));
      spline.add(p);
    }
  }


  template<class S, class V> 
    SplineEditor<S, V>&  SplineEditor<S, V>::hide() {
    this->is_visible = false;
    this->static_text->setTextVisible(this->label_dx, false);
    return *this;
  }

  template<class S, class V> 
    SplineEditor<S, V>&  SplineEditor<S, V>::show() {
    this->is_visible = true;
    this->static_text->setTextVisible(this->label_dx, true);
    return *this;
  }

  //typedef SplineEditor<Spline> SplineEditor;


  /*
  // This Cubic Bezier spline, works with a set of points where points
  // and handles are added in a successive way: point, handle, handle, point
  // handle, handle, point, handle, handle, point, etc...
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  int num_curves = 1 + (float(points.size())-4.0f)/3.0f;
  float curve_p = t * num_curves;
  int curve_num = curve_p;
  t = curve_p - curve_num;
	
  int end_dx = 3 + (curve_num-1) * 3; // [num_start_points] + (curvenum-1) * [num_points_successive_curves], we remove 1 from the start, which is 4 so we have a array index

  int dx_d = end_dx+3;
  int dx_c = end_dx+2;
  int dx_b = end_dx+1;
  int dx_a = end_dx;

  //printf("end dx: %d, (%d, %d, %d, %d)\n", end_dx, dx_a, dx_b, dx_c, dx_d);
  int dx = end_dx;
  CurvePoint& p0 = points[dx]; // a
  CurvePoint& p1 = points[dx + 1]; // b 
  CurvePoint& p2 = points[dx + 2]; // c
  CurvePoint& p3 = points[dx + 3]; // d 

  CurvePoint a,b,c,p;
	
  c.x = 3 * (p1.x - p0.x);
  c.y = 3 * (p1.y - p0.y);
  b.x = 3 * (p2.x - p1.x) - c.x;
  b.y = 3 * (p2.y - p1.y) - c.y;
  a.x = p3.x - p0.x - c.x - b.x;
  a.y = p3.y - p0.y - c.y - b.y;

  float t2 = t*t;
  float t3 = t*t*t;
  x = a.x * t3 + b.x * t2 + c.x * t + p0.x;
  y = a.y * t3 + b.y * t2 + c.y * t + p0.y;
  */
	
	
  /*
  // This version is an implementation of Cubic Beziers, where you we assume
  // that multiple curves are added. So everytime 4 points.
  float curve_p = t * (points.size()/4);
  int curve_num = curve_p;
  t = curve_p - curve_num;

  int dx = curve_num * 4;
  CurvePoint& p0 = points[dx]; // a
  CurvePoint& p1 = points[dx + 1]; // b 
  CurvePoint& p2 = points[dx + 2]; // c
  CurvePoint& p3 = points[dx + 3]; // d 

  CurvePoint a,b,c,p;
	
  c.x = 3 * (p1.x - p0.x);
  c.y = 3 * (p1.y - p0.y);
  b.x = 3 * (p2.x - p1.x) - c.x;
  b.y = 3 * (p2.y - p1.y) - c.y;
  a.x = p3.x - p0.x - c.x - b.x;
  a.y = p3.y - p0.y - c.y - b.y;

  float t2 = t*t;
  float t3 = t*t*t;
  x = a.x * t3 + b.x * t2 + c.x * t + p0.x;
  y = a.y * t3 + b.y * t2 + c.y * t + p0.y;
  */


} // namespace buttons

#endif
