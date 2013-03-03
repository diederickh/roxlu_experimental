#include <iostream>
#include <string>
#include <roxlu/Roxlu.h>
#include <libconfig.h++> // you must include this

int main() {
  libconfig::Config config;

  // LOAD CONFIG
  try {
    config.readFile(rx_to_data_path("settings.txt").c_str());
  }
  catch(libconfig::ConfigException& ex) {
    RX_ERROR(("Error loading settings file: %s", ex.what()));
    return EXIT_FAILURE;
  }

  // THE VARS WE WANT TO RETRIEVE
  std::string upload_url;
  std::string src_path;
  int delete_after_days = 0;
  bool must_delete = false;
  

  // RETRIEVE VARS
  try {
    config.lookupValue("upload_url", upload_url);
    config.lookupValue("src_path", src_path);
    config.lookupValue("delete_after_days", delete_after_days);
    config.lookupValue("must_delete", must_delete);
  }
  catch(libconfig::ConfigException& ex) {
    RX_ERROR(("Error getting settings: %s", ex.what()));
    return EXIT_FAILURE;
  }

  printf("upload_url: %s\n", upload_url.c_str());
  printf("src_path: %s\n", src_path.c_str());
  printf("delete_after_days: %d\n", delete_after_days);
  printf("must_delete: %d\n", must_delete);
  return 1;
};
