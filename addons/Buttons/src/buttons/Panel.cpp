#include <buttons/Panel.h>
#include <buttons/Buttons.h>
#include <algorithm>

namespace buttons {

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  struct ButtonsFindByName {	
    ButtonsFindByName(const string n):n(n){}
    bool operator()(Buttons* b) {
      return b->getName() == n;
    }
    string n;
  };

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	

  Panel::Panel(const string& defaultGuiName, int w, int h) 
    :w(w)
    ,h(h)
    ,active_gui(NULL)
    ,content_h(0)
    ,content_x(0)
    ,content_y(0)
    ,step_dx(0)
  {
    addButtons(defaultGuiName);
  }

  Panel::~Panel() {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      delete *it;
    }
  }

  void Panel::update() {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->update();
    }
  }

  void Panel::draw() {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->draw();
    }
  }

  void Panel::onMouseMoved(int mx, int my) {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->onMouseMoved(mx, my);
    }
  }

  void Panel::onMouseDown(int mx, int my) {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->onMouseDown(mx, my);
    }
  }

  void Panel::onMouseUp(int mx, int my) {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->onMouseUp(mx, my);
    }
  }

  // -----------------------------------------------------------------------------

  void Panel::toggleVisible() {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->toggleVisible();
    }
  }

  void Panel::hide() {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->hide();
    }
  }

  void Panel::show() {
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      (*it)->show();
    }
  }

  // -----------------------------------------------------------------------------
  Buttons& Panel::addButtons(const string& title) {
    Buttons* bt = new Buttons(title, w);
    guis.push_back(bt);
    active_gui = bt;
    active_gui->addListener(this);
    if(guis.size() > 1) {
      bt->setLock(true);
    }
    return *bt;
  }


  Sliderf& Panel::addFloat(const string& label, float& value) {
    assert(active_gui != NULL);
    return active_gui->addFloat(label, value);
  }
  
  SliderRangef& Panel::addFloatRange(const string& label, float* value) {
    assert(active_gui != NULL);
    return active_gui->addFloatRange(label, value);
  }

  Slideri& Panel::addInt(const string& label, int& value) {
    assert(active_gui != NULL);
    return active_gui->addInt(label, value);
  }

  SliderRangei& Panel::addIntRange(const string& label, int* value) {
    assert(active_gui != NULL);
    return active_gui->addIntRange(label, value);
  }

  Pad<int>& Panel::addInt2(const string& label, int* value) {
    assert(active_gui != NULL);
    return active_gui->addInt2(label, value);
  }

  Pad<float>& Panel::addFloat2(const string& label, float* value) {
    assert(active_gui != NULL);
    return active_gui->addFloat2(label, value);
  }

  Vector<float>& Panel::addVec2f(const string& label, float* value) {
    assert(active_gui != NULL);
    return active_gui->addVec2f(label, value);
  }

  Rectangle<int>& Panel::addRectanglei(const string& label, int* value) {
    assert(active_gui != NULL);
    return active_gui->addRectanglei(label, value);
  }

  Rectangle<float>& Panel::addRectanglef(const string& label, float* value) {
    assert(active_gui != NULL);
    return active_gui->addRectanglef(label, value);
  }

  ColorPicker& Panel::addColor(const string& label, float* value) {
    assert(active_gui != NULL);
    return active_gui->addColor(label, value);
  }

  Toggle& Panel::addBool(const string& label, bool& value) {
    assert(active_gui != NULL);
    return active_gui->addBool(label, value);
  }

  Button& Panel::addButton(const std::string& label, int id, button_on_click_callback cb, void* user) {
    assert(active_gui != NULL);
    return active_gui->addButton(label, id, cb, user);
    //buttons::Button<T>* el = new Button<T>(id, cb, createCleanName(label));
    //buttons::Button* el = new Button(id, cb, user, createCleanName(label));
    //addElement(el, label);
    //return *el;
  }

  // get a Buttons
  Buttons& Panel::getButtons(const string& name) {
    vector<Buttons*>::iterator it = std::find_if(guis.begin(), guis.end(), ButtonsFindByName(name));
    assert(it != guis.end());
    return **it;
  }
  
  Element* Panel::getElement(const string label) {
    assert(active_gui != NULL);
    return active_gui->getElement(label);
  }

  // set the current active gui
  void Panel::select(const string& name) {
    assert(active_gui != NULL);
    active_gui = &getButtons(name);
  }

  void Panel::setColor(const float hue, float sat, float bright, float a) {
    assert(active_gui != NULL);
    active_gui->setColor(hue, sat, bright, a);
  }

  // Resize / positioning / open-close
  // -----------------------------------------------------------------------------
  int Panel::getContentHeight() {
    int new_h = 0;
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      new_h += (*it)->getPanelHeight();
    }
    return new_h;
  }

  void Panel::position() {
    int yy = content_y;
    int xx = content_x;
    (*guis.begin())->getPosition(xx, yy);
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      Buttons& b = **it;
      b.setPosition(xx, yy);
      yy += b.getPanelHeight();
    }
  }

  void Panel::step() {
    step_dx = ++step_dx % guis.size();
    for(int i = 0; i < guis.size(); ++i) {
      if(i == step_dx) {
        guis[i]->open();
      }
      else {
        guis[i]->close();
      }
    }
  }

  void Panel::save() {
    std::string filepath = getDataPath();;

    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      Buttons& b = **it;
      string filename = "gui_" +b.getName() +".cfg";
      b.save(filepath + filename);
    }
  }

  void Panel::load() {
    std::string filepath = getDataPath();;
    for(vector<Buttons*>::iterator it = guis.begin(); it != guis.end(); ++it) {
      Buttons& b = **it;
      string filename = "gui_" +b.getName() +".cfg";
      b.load(filepath + filename);
    }
  }

  // Event handling
  // -----------------------------------------------------------------------------
  void Panel::onEvent(ButtonsEventType event, const Buttons& bt, const Element* target, void* targetData) {
    // position change?
    int xx = 0;
    int yy = 0;
    (*guis.begin())->getPosition(xx, yy);
		
    if(xx != content_x || yy != content_y) {
      content_x = xx;
      content_y = yy;
      position();
      return;
    }
	
    // size changed?
    int new_h = getContentHeight();;
    if(new_h != content_h) {
      content_h = new_h;
      position();
    }
  }


} // buttons
