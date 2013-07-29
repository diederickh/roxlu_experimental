#ifndef ROXLU_JANSSON_H
#define ROXLU_JANSSON_H

#include <jansson.h>
#include <string>
#include <vector>

struct JanssonPathSegment {
  std::string name;
};


class Jansson {
 public:
  Jansson();
  ~Jansson();
  bool load(std::string filename, bool datapath);
  bool getString(std::string path, std::string& result);
  bool getInt(std::string path, int& result);
  bool getDouble(std::string path, double& result);
  bool getFloat(std::string path, float& result);
  bool getBool(std::string path, bool& result);
  bool getJSONElement(std::string path, json_t** result);

  bool parse(std::string path, std::vector<JanssonPathSegment>& result);
 public:
  json_t* root;
};

inline bool Jansson::getFloat(std::string path, float& result) {
  double r = 0.0;
  
  if(!getDouble(path, r)) {
    return false;
  }
  result = (float)r;
  return true;
}

#endif
