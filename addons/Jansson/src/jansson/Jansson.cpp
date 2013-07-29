#include <jansson.h>
#include <jansson/Jansson.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <sstream>

Jansson::Jansson() 
  :root(NULL)
{
}

Jansson::~Jansson() {

  if(root) {
    json_decref(root);
    root = NULL;
  }

}

bool Jansson::load(std::string filename, bool datapath) {

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  if(!rx_file_exists(filename)) {
    RX_ERROR("Cannot find the file: `%s`", filename.c_str());
    return false;
  }

  json_error_t err;
  root = json_load_file(filename.c_str(), 0, &err);
  if(!root) {
    RX_ERROR("Cannot load the json file: `%s`, error: %s (%d)", filename.c_str(), err.text, err.line);
    return false;
  }

  return true;
}

bool Jansson::getString(std::string path, std::string& result) {

  json_t* json_el;
  if(!getJSONElement(path, &json_el)) {
    return false;
  }
  
  if(!json_is_string(json_el)) {
    RX_ERROR("`%s` is not a string value.", path.c_str());
    return false;
  }

  result = json_string_value(json_el);
  return true;

}

bool Jansson::getInt(std::string path, int& result) {

  json_t* json_el;
  if(!getJSONElement(path, &json_el)) {
    return false;
  }

  if(!json_is_integer(json_el)) {
    RX_ERROR("`%s` is not an integer value.", path.c_str());
    return false;
  }

  result = json_integer_value(json_el);
  return true;
}

bool Jansson::getDouble(std::string path, double& result) {

  json_t* json_el;
  if(!getJSONElement(path, &json_el)) {
    return false;
  }

  if(!json_is_number(json_el)) {
    RX_ERROR("`%s` is not an number value.", path.c_str());
    return false;
  }

  result = json_number_value(json_el);
  return true;
}

bool Jansson::getBool(std::string path, bool& result) {

  json_t* json_el;
  if(!getJSONElement(path, &json_el)) {
    return false;
  }

  if(!json_is_boolean(json_el)) {
    RX_ERROR("`%s` is not an boolean value.", path.c_str());
    return false;
  }

  result = json_is_true(json_el);
  return true;
}


bool Jansson::getJSONElement(std::string path, json_t** result) {

  *result = NULL;

  std::vector<JanssonPathSegment> els;
  if(!parse(path, els)) {
    RX_ERROR("Cannot parse path.");
    return false;
  }

  size_t count = els.size();
  size_t dx = 0;
  json_t* json_el = root;

  for(std::vector<JanssonPathSegment>::iterator it = els.begin(); it != els.end(); ++it) {
    JanssonPathSegment& ps = *it;
    if((dx + 1) >= count) {
      json_el = json_object_get(json_el, ps.name.c_str());
      if(!json_el) {
        RX_ERROR("Element not found: `%s`, from path: `%s`", ps.name.c_str(), path.c_str());
        return false;
      }
      if(json_el) {
        *result = json_el;
        return true;
      }
    }
    else {
      json_el = json_object_get(json_el, ps.name.c_str());
      if(!json_is_object(json_el)) {
        RX_ERROR("The element %s is not an json object.", ps.name.c_str());
        break;
      }
    }
    ++dx;
  }
  return false;
}

bool Jansson::parse(std::string path, std::vector<JanssonPathSegment>& result) {

  if(path.size() == 0) {
    RX_ERROR("Cannot parse empty path");
    return false;
  }

  if(path[0] != '/') {
    RX_ERROR("All paths should start from the root, so with a `/`");
    return false;
  }

  std::string el;
  std::stringstream ss(path);
  while(std::getline(ss, el, '/')) {
    if(!el.size()) {
      continue;
    }

    JanssonPathSegment ps;
    ps.name = el;
    result.push_back(ps);
  }

  return result.size() > 0;
}
