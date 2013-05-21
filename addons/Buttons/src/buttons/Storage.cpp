#include <buttons/Storage.h>
#include <buttons/Buttons.h>

namespace buttons {

  bool Storage::save(const string& file, Buttons* buttons) {
    config_init(&cfg);

    Buttons& b = *buttons;    
    config_setting_t* gui_setting;
    config_setting_t* el_setting;
    config_setting_t* el_group;
    config_setting_t* root = config_root_setting(&cfg);
    config_setting_t* gui = config_setting_add(root, "gui", CONFIG_TYPE_GROUP);
    
    gui_setting = config_setting_add(gui, "x", CONFIG_TYPE_INT);
    config_setting_set_int(gui_setting, b.x);

    gui_setting = config_setting_add(gui, "y", CONFIG_TYPE_INT);
    config_setting_set_int(gui_setting, b.y);

    gui_setting = config_setting_add(gui, "w", CONFIG_TYPE_INT);
    config_setting_set_int(gui_setting, b.w);
    
    el_group = config_setting_add(root, "elements", CONFIG_TYPE_GROUP);

    vector<Element*>::iterator it = b.elements.begin();
    while(it != b.elements.end()) {
      Element& e = *(*it);
      if(!e.canSave()) {
        ++it;
        continue;
      }
      e.save(el_group);
      ++it;
    }
		
    if(!config_write_file(&cfg, file.c_str())) {
      printf("ERROR: cannot save settings file");
    }
    config_destroy(&cfg);
    return true;
  }

  bool Storage::load(const string& file, Buttons* buttons) {
    config_t cfg;

    config_init(&cfg);
    if(!config_read_file(&cfg, file.c_str())) {
      printf("ERROR: cannot load config file: %s\n", file.c_str());
      return false;
    }

    Buttons& b = *buttons;
    config_lookup_int(&cfg, "gui.x", &b.x);
    config_lookup_int(&cfg, "gui.y", &b.y);
    config_lookup_int(&cfg, "gui.w", &b.w);

    b.setPosition(b.x, b.y);

    config_setting_t* el_group = config_lookup(&cfg, "elements");
    if(!el_group) {
      printf("WARNING: cannot find elements\n");
      return true;
    }
    
    vector<Element*>::iterator it = b.elements.begin();
    while(it != b.elements.end()) {
      Element& e = *(*it);
      if(!e.canSave()) {
        ++it;
        continue;
      }
      e.load(el_group);
      ++it;
    }
    return true;
  }

} // namespace buttons


