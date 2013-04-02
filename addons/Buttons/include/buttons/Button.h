#ifndef ROXLU_BUTTONS_BUTTONH
#define ROXLU_BUTTONS_BUTTONH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

  typedef void(*button_on_click_callback)(int id, void* user);

  class Buttons;

  class Button : public Element {
  public:
  Button(unsigned int id, button_on_click_callback clickCallback, void* user, const string& name) 
    :Element(BTYPE_BUTTON, name)
      ,id(id)
      ,cb_click(clickCallback)
      ,cb_user(user)
      ,label_dx(0)
      {
        h = 20;		
        event_data = (void*)&this->id;
      }
	
    ~Button() {
    }

    Button& setColor(const float hue, float sat = 0.2f, float bright = 0.27f, float a = 1.0f) {
      Element::setColor(hue, sat, bright, a);
      HSL_to_RGB(col_hue, col_sat, col_bright - 0.2,  &button_bg_color[0], &button_bg_color[1], &button_bg_color[2]);
      HSL_to_RGB(col_hue, col_sat, col_bright - 0.2,  &toggle_off_color[0], &toggle_off_color[1], &toggle_off_color[2]);
      HSL_to_RGB(col_hue, col_sat, col_bright + 0.2,  &toggle_on_color[0], &toggle_on_color[1], &toggle_on_color[2]);
      BSET_COLOR(button_bg_color, 0.0f, 0.0f, 0.0f, 0.24f);
      toggle_on_color[3] = a;
      toggle_off_color[3] = a;
      return *this;
    }
	
    void generateStaticText() {
      label_dx = static_text->add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
    }

    void updateTextPosition() {
      static_text->setTextPosition(label_dx, x+4, y+2);
    }
	
    void generateVertices(ButtonVertices& vd) {
      int bt_size = 10;
      int bt_x = (x+w) - (bt_size+8);
      int bt_y = y+3;
      int bt_bg_x = bt_x + 2;
      int bt_bg_y = bt_y + 2;
      if(is_mouse_down_inside) {
        bt_x = bt_bg_x;
        bt_y = bt_bg_y;
      }
		
      buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
      buttons::createRect(vd, bt_bg_x, bt_bg_y, bt_size, bt_size, button_bg_color, button_bg_color);
      buttons::createRect(vd, bt_x, bt_y, bt_size, bt_size, toggle_on_color, toggle_off_color);
    }


    void onMouseDown(int mx, int my) {
      needsRedraw();
    }
	
    void onMouseUp(int mx, int my) {
      needsRedraw();
    }
	
    void onMouseEnter(int mx, int my) {
      bg_top_color = col_bg_top_hover;
      bg_bottom_color = col_bg_bottom_hover;
      needsRedraw();
    }
	
    void onMouseLeave(int mx, int my) {
      bg_top_color = col_bg_default;
      bg_bottom_color = col_bg_default;
      needsRedraw();
    }
	
    void onMouseClick(int mx, int my) {
      if(is_enabled && cb_click) {
        cb_click(id, cb_user);
      }

      flagValueChanged();
    }

    void setValue(void* v) {
      if(is_enabled && cb_click) {
        cb_click(id, cb_user);
      }
    }
	
    void save(std::ofstream& ofs) {	
    }

    void load(std::ifstream& ifs) { 
    }

    bool canSave() { 
      return false;
    }
		
    Button& hide() {
      is_visible = false;
      static_text->setTextVisible(label_dx, false);
      return *this;
    }
	
    Button& show() {
      is_visible = true;
      static_text->setTextVisible(label_dx, true);
      return *this;
    }

    bool serializeScheme(ButtonsBuffer& buffer) {
      buffer.addUI32(id);
      return true;
    }

    int label_dx;
    float toggle_on_color[4];
    float toggle_off_color[4];
    float button_bg_color[4];	
    unsigned int id;
    button_on_click_callback cb_click;
    void* cb_user;

  };

} // namespace buttons

#endif
