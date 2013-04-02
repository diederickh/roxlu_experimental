/**
 * Radio 
 * -----
 * Creates a list of radios that you can toggle. 
 * 
 *    Example:
 *    -------
 *    vector<string> options;
 *    options.push_back("Option 1");
 *    options.push_back("Option 2");
 *    options.push_back("Option 3");
 *    gui.addRadio<Simulation>("Options", 1, this, options, test_option); 
 *    gui.addButton<Simulation>("Button", 0, this);
 * 
 */
#ifndef ROXLU_BRADIOH
#define ROXLU_BRADIOH

#include <string>
#include <vector>

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Toggle.h>

using std::string;
using std::vector;

namespace buttons {

  template<class T>

    class Radio : public Element {
  public:	
    Radio(int id, vector<string> options, int& selected, const string& name, T* cb); 
    ~Radio();

    void addOption(Toggle* toggle);
    vector<Toggle*>& getOptions();

    void generateStaticText();
    void updateTextPosition();
    void generateVertices(ButtonVertices& shapeVertices);
	
    void getChildElements(vector<Element*>& elements);
    void positionChildren();

    void onMouseDown(int mx, int my);
    void onMouseUp(int mx, int my);
    void onMouseEnter(int mx, int my);
    void onMouseLeave(int mx, int my);	
    void onMouseClick(int mx, int my);
    void onSaved();
    void onLoaded();
	
    void save(std::ofstream& ofs);
    void load(std::ifstream& ifs);
    bool canSave();
    bool serializeScheme(ButtonsBuffer& buffer);
	
    Radio<T>& hide();
    Radio<T>& show();

    bool updateSelected();
    void setValue(void* v); // see client/server
    void setValue(int index); 

    int label_dx;
    vector<string> options;
    T* cb;

    int& selected;
    bool* values; // array of bool for each toggle
    vector<Toggle*> toggles;
    int id;
    int h_default; 
  };

  template<class T>
    Radio<T>::Radio(int id, vector<string> options, int& selected, const string& name, T* cb) 
    :Element(BTYPE_RADIO, name)
    ,options(options)
    ,cb(cb)
    ,id(id)
    ,selected(selected)
    ,label_dx(0)
    ,values(NULL)
    {
      h = 20;
      BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
      BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
      values = new bool[options.size()];
      event_data = (void*)&id;
      h_default = h;

      for(int i = 0; i < options.size(); ++i) {
        Toggle* t = new Toggle(values[i], buttons_create_clean_name(options[i]));
        t->label = options[i];
        addOption(t);
      }
      printf("Selected value: %d\n", selected);
      setValue(selected);
    }

  template<class T>
    Radio<T>::~Radio() {
  }

  template<class T>
    void Radio<T>::addOption(Toggle* toggle) {
    toggles.push_back(toggle);
    toggle->is_radio = true;
    toggle->is_child = true;
    toggle->needsRedraw();
  }

  template<class T> 
    void Radio<T>::getChildElements(vector<Element*>& elements) {
    for(vector<Toggle*>::iterator it = toggles.begin(); it != toggles.end(); ++it) {
      elements.push_back((*it));
      printf("Get child elements in radio!\n");
    }
  }

  template<class T>
    void Radio<T>::positionChildren() {
    h = h_default;
    float yy = y + h_default; 
    for(vector<Toggle*>::iterator it = toggles.begin(); it != toggles.end(); ++it) {
      Toggle& toggle = **it;
      toggle.y = yy;
      toggle.x = x;
      toggle.w = w;
      yy += toggle.h;
      h += toggle.h;
    }
  }

  template<class T>
    vector<Toggle*>& Radio<T>::getOptions() {
    return toggles;
  }

  template<class T>
    void Radio<T>::generateStaticText() {
    label_dx = static_text->add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
  }

  template<class T>
    void Radio<T>::updateTextPosition() {
    static_text->setTextPosition(label_dx, x+4, y+2);
  }

  template<class T>
    void Radio<T>::generateVertices(ButtonVertices& vd) {
    buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
  }

  template<class T>
    void Radio<T>::onMouseDown(int mx, int my) {
    bool was_inside = updateSelected();
    if(was_inside) {
      (*cb)(id);
    }	
  }

  template<class T>
    bool Radio<T>::updateSelected() {
    bool was_inside = false;
    for(int i = 0; i < toggles.size(); ++i) {
      Toggle& toggle = *toggles[i];
      if(toggle.is_mouse_inside && was_inside == false) {
        was_inside = true;
        selected = i;
        event_data = (void*)&selected;
      }
    }
	
    if(was_inside) {
      for(int i = 0; i < toggles.size(); ++i) {
        Toggle& toggle = *toggles[i];
        if(i != selected) {
          toggle.value = false;
          toggle.needsRedraw();
        }
      }
    }
    return was_inside;
  }


  template<class T>
    void Radio<T>::onMouseUp(int mx, int my) {
  }

  template<class T>
    void Radio<T>::onMouseEnter(int mx, int my) {
  }

  template<class T>
    void Radio<T>::onMouseLeave(int mx, int my) {
  }

  template<class T>
    void Radio<T>::onMouseClick(int mx, int my) {
  }

  template<class T>
    void Radio<T>::onSaved() {
  }

  template<class T>
    void Radio<T>::onLoaded() {

    for(int i = 0; i < toggles.size(); ++i) {
      Toggle& toggle = *toggles[i];

      if(toggle.value) {
        selected = i;
        //printf("Loaded radio. selected is: %d\n", selected);
        break;
      }
    }
  }

  // Implemented for client/server
  template<class T>
    void Radio<T>::setValue(void* v) {
    int* ip = (int*)v;
    selected = *ip; 
    setValue(selected);
  }
 
  template<class T>
    void Radio<T>::setValue(int selectedIndex) {
    for(int i = 0; i < toggles.size(); ++i) {
      Toggle& toggle = *toggles[i];
      toggle.setValue(false);
      if(i == selectedIndex) {
        toggle.setValue(true);
        toggle.needsRedraw();
        printf("ok set %d to true, %d\n", i, selectedIndex);
      }
    }
    needsRedraw();
    //	(*cb)(id);
  }

  template<class T>
    void Radio<T>::save(std::ofstream& ofs) {}

  template<class T>
    void Radio<T>::load(std::ifstream& ifs) { }

  template<class T>
    bool Radio<T>::canSave() { 
    // child element store themselves
    return false; 
  }

  template<class T>
    Radio<T>& Radio<T>::hide() {
    is_visible = false;
    static_text->setTextVisible(label_dx, false);
    return *this;
  }

  template<class T>
    Radio<T>& Radio<T>::show() {
    is_visible = true;
    static_text->setTextVisible(label_dx, true);
    return *this;
  }

  template<class T>
    bool Radio<T>::serializeScheme(ButtonsBuffer& buffer) {
    buffer.addUI32(id);
    buffer.addUI32(selected);
    buffer.addUI32(options.size());
    for(int i = 0; i < options.size(); ++i) {
      buffer.addString(options[i]);
    }
    for(int i = 0; i < options.size(); ++i) {
      //			buffer.addByte((values[i] == true) ? 1 : 0);
    }
    /*					Radio<Server>* radio = static_cast<Radio<Server>* >(el); 
					int* tmp_val = (int*)el->event_data;
					scheme.addUI32(*tmp_val);
					scheme.addUI32(radio->options.size());
					printf("OPTINS: %zu\n", radio->options.size());
					for(vector<string>::const_iterator it = radio->options.begin(); it != radio->options.end(); ++it) {
                                        scheme.addString(*it);
					}
					for(int i = 0; i < radio->options.size(); ++i) {
                                        scheme.addByte((radio->values[i]) == true ? 1 : 0);
					}
    */

    return true;

  }

} // namespace buttons

#endif
