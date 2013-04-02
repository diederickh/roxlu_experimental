#include <buttons/Toggle.h>
#include <buttons/Buttons.h>

namespace buttons {
	
  Toggle::Toggle(bool& val, const string& name) 
    :value(val)
    ,Element(BTYPE_TOGGLE, name)
    ,label_dx(0)
    ,is_radio(false) // not used yet... should draw a circle instead!
  {
    h = 20;
  }

  Toggle::~Toggle() {
  }

  void Toggle::generateStaticText() {
    label_dx = static_text->add(x+20, y+2, label, 0.9, 0.9, 0.9, 0.9);
  }

  void Toggle::updateTextPosition() {	
    static_text->setTextPosition(label_dx, x+20, y+2);
  }

  void Toggle::generateVertices(ButtonVertices& vd) { 
    buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
    float* c = toggle_off_color;
    if(value) {
      c = toggle_on_color;
    }
	
    buttons::createRect(vd, x+5, y+5, 10, 10, c, c);
  }

  void Toggle::onMouseDown(int mx, int my) {
  }

  void Toggle::onMouseUp(int mx, int my) {
  }

  void Toggle::onMouseEnter(int mx, int my) {
    bg_top_color = col_bg_top_hover;
    bg_bottom_color = col_bg_bottom_hover;
    needsRedraw();
  }

  void Toggle::onMouseLeave(int mx, int my) {
    bg_top_color = col_bg_default;
    bg_bottom_color = col_bg_default;
    needsRedraw();
  }

  void Toggle::onMouseClick(int mx, int my) {
    if(is_radio) {
      if(!value) { // only set to true and redraw when we arent already true
        value = true;
        needsRedraw();
        flagValueChanged();
      }
    }
    else {
      value = !value;
      needsRedraw();
      flagValueChanged();
    }
  }

  bool Toggle::canSave() {
    return true;
  }

  void Toggle::save(ofstream& ofs) {
    char stored_data = (value) ? 'y' : 'n';
    size_t data_size = 1;
    ofs.write((char*)&data_size, sizeof(data_size));
    ofs.write(&stored_data, 1);
  }
  
  void Toggle::save(config_setting_t* setting) {
    config_setting_t* cfg_el = config_setting_add(setting, buttons_create_clean_name(name).c_str(), CONFIG_TYPE_BOOL);
    config_setting_set_bool(cfg_el, value);
  }

  void Toggle::load(std::ifstream& ifs) {
    char stored_data = '?';
    ifs.read(&stored_data, 1);
    value = (stored_data == 'y');
    needsRedraw();
  }

  void Toggle::load(config_setting_t* setting) {
    std::string cname = buttons_create_clean_name(name);
    config_setting_t* cfg = config_setting_get_member(setting, cname.c_str());
    if(!cfg) {
      printf("ERROR: cannot find toggle setting value: %s\n", cname.c_str());
      return;
    }
    value = config_setting_get_bool(cfg);
    needsRedraw();
  }

  Toggle& Toggle::hide() {
    is_visible = false;
    static_text->setTextVisible(label_dx, false);
    return *this;
  }

  Toggle& Toggle::show() {
    is_visible = true;
    static_text->setTextVisible(label_dx, true);
    return *this;
  }

  Toggle& Toggle::setColor(const float hue, float sat, float bright, float a) {
    Element::setColor(hue, sat, bright, a);
    HSL_to_RGB(col_hue, col_sat, col_bright - 0.1,  &toggle_off_color[0], &toggle_off_color[1], &toggle_off_color[2]);
    HSL_to_RGB(col_hue, col_sat, col_bright + 0.4,  &toggle_on_color[0], &toggle_on_color[1], &toggle_on_color[2]);
    toggle_on_color[3] = a;
    toggle_off_color[3] = a;
    return *this;
  }

  bool Toggle::serializeScheme(ButtonsBuffer& buffer) {
    buffer.addByte(value ? 1 : 0);
    return true;
  }

} // namespace buttons
