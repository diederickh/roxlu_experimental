#ifndef ROXLU_BUTTONS_STORAGEH
#define ROXLU_BUTTONS_STORAGEH

#include <stdio.h>
#include <string>
#include <fstream>
#include <libconfig.h>

using std::string;

namespace buttons {

  class Buttons;

  class Storage {

  public: 
    Storage() {}
    bool save(const string& file, Buttons* b);
    bool load(const string& file, Buttons* b);
  private:
    config_t cfg;
  };

} // namespace buttons
#endif
