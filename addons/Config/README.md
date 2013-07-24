Config
=======
This is an empty addon which adds the correct compiler 
settings for libconfig: http://www.hyperrealm.com/libconfig/

_Example_
````c++
#include <libconfig.h++> // you must include this
#include "ApplicationConfig.h"
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>

bool ApplicationConfig::load(std::string filename, bool datapath) {

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  libconfig::Config config;

  // LOAD CONFIG
  try {
    config.readFile(filename.c_str());
  }
  catch(libconfig::ConfigException& ex) {
    RX_ERROR("Error loading settings file: %s", ex.what());
    return false;
  }

  // PARSE CONFIG
  if(!config.lookupValue("app.scene_dirname", scene_dirname);
    || !config.lookupValue("app.card_filename", card_filename);
    || !config.lookupValue("app.num_cards", num_cards);
  {   
    RX_ERROR("Error finding settings: %s", ex.what());
    return false;
  }

  return true;
}

void ApplicationConfig::print() {
  RX_VERBOSE("config.scene_dirname: %s", scene_dirname.c_str());
  RX_VERBOSE("config.card_filename: %s", card_filename.c_str());
  RX_VERBOSE("config.num_cards: %d",  num_cards);
}

````

_Creating a config file_

````c++

#include <libconfig.h++> 

bool YouTube::save() {

  libconfig::Config cfg;
  libconfig::Setting& root = cfg.getRoot();
  
  root.add("refresh_token", libconfig::Setting::TypeString) = refresh_token.c_str();
  root.add("access_token", libconfig::Setting::TypeString) = access_token.c_str();
  root.add("token_timeout", libconfig::Setting::TypeInt64) = (long long int)(token_timeout);
  
  try {
    cfg.writeFile(rx_to_data_path("youtube.cfg").c_str());
  }
  catch (const libconfig::FileIOException& ex) {
    RX_ERROR("Error while saving state of the youtube client.");
    return false;
  }
  
  return true;
}


````

