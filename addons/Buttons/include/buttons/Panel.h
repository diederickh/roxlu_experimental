#ifndef ROXLU_BUTTONS_PANELH
#define ROXLU_BUTTONS_PANELH

#include <assert.h>
#include <string>
#include <vector>
#include <map>

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Slider.h>
#include <buttons/Toggle.h>
#include <buttons/Button.h>
#include <buttons/Radio.h>
#include <buttons/Spline.h>
#include <buttons/Color.h>
#include <buttons/Pad.h>
#include <buttons/Rectangle.h>
#include <buttons/Storage.h>
#include <buttons/Buttons.h>
#include <roxlu/core/Utils.h>

using std::string;
using std::vector;
using std::map;

using namespace roxlu;

namespace buttons {

  class Panel : public ButtonsListener {
  public:
    Panel(const string& defaultGuiName, int w, int h);
    ~Panel();

    void setDataPath(std::string path);  // set the path into which we will store the GUI settings. if not set we use the executable path 
    std::string getDataPath(); // returns the path where the save data will be stored
    void update();
    void draw();

    void toggleVisible();
    void hide();
    void show();
	
    Toggle& addBool(const string& label, bool& value);
    Slideri& addInt(const string& label, int& value);
    Sliderf& addFloat(const string& label, float& value);
    SliderRangef& addFloatRange(const string& label, float* value);
    SliderRangei& addIntRange(const string& label, int* value);
    ColorPicker& addColor(const string& label, float* value); // pointer to arary with 4 values
    Pad<int>& addInt2(const string& label, int* value); // pointer to array with 2 values
    Pad<float>& addFloat2(const string& label, float* value); // pointer to array with 2 values
    Rectangle<int>& addRectanglei(const string& label, int* value); // pointer to array with 4 entries (topleft, bottomright)
    Rectangle<float>& addRectanglef(const string& label, float* value); // pointer to array with 4 entries (topleft, bottomright)
    Vector<float>& addVec2f(const string& label, float* value);
    Button& addButton(const std::string& label, int id, button_on_click_callback cb, void* user);

    template<class T>
      Radio<T>& addRadio(const string& label, int id, T* cb, vector<string> options, int& value) {
      return active_gui->addRadio<T>(label, id, cb, options, value);
    }

    Buttons& addButtons(const string& title);
    Element* getElement(const string label);
	
    void save();
    void load();
	
    void onMouseMoved(int mx, int my);
    void onMouseDown(int mx, int my);
    void onMouseUp(int mx, int my);
	
    Buttons& getButtons(const string& name);

    void step(); // open/close gui one by one	
    void select(const string& name);
    void setColor(const float hue, float sat = 0.2f, float bright = 0.27f, float a = 1.0f);
		
    void onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target, void* targetData);
	
    int getContentHeight();
    void setPosition(int x, int y);
    void position();
    void lock();
    void unlock();
	
    int content_y;
    int content_x;
    int content_h;
    int w;
    int h;
    int step_dx;
	
    vector<Buttons*> guis;
    Buttons* active_gui;
    std::string data_path;
  };

  inline void Panel::setDataPath(std::string path) {

#if !defined(_WIN32) 
    if(path[path.size()-1] != '/') {
      path.push_back('/');
    }
#else
    if(path[path.size()-1] != '\\') {
      path.push_back('\\');
    }
#endif

    data_path = path;
  }

  inline std::string Panel::getDataPath() {
    std::string filepath = "";
    if(data_path.size()) {
      filepath = data_path;
    }
    else {
      filepath = rx_to_data_path("./");
    }
    return filepath;
  }

  inline void Panel::setPosition(int x, int y) {
    assert(active_gui);
    active_gui->setPosition(x,y);
  }

  inline void Panel::lock() {
    assert(active_gui);
    active_gui->lock();
  }

  inline void Panel::unlock() {
    assert(active_gui);
    active_gui->unlock();
  }
} // buttons

#endif
