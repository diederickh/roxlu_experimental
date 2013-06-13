#ifndef ROXLU_HTTP_PARAMETER_H
#define ROXLU_HTTP_PARAMETER_H

#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <http/HTTPTypes.h>

class HTTPParameter {
 public:
  HTTPParameter(HTTPParameterType type = HTTP_PARAMETER_NAME_VALUE);
  HTTPParameter(const HTTPParameter& o);
  HTTPParameter& copy(const HTTPParameter& o);
  HTTPParameter& operator=(const HTTPParameter& o);

  template<class T> 
    HTTPParameter(std::string name, T v, HTTPParameterType type = HTTP_PARAMETER_NAME_VALUE)
    :name(name)
    ,type(type)
    {
      setValue(v);
    }
  
  ~HTTPParameter();

  template<class T> void setValue(T v) {
    std::stringstream ss;
    ss << v;
    value = ss.str();
  }

  size_t getNumFiles();
  void setType(HTTPParameterType type);
  void setName(std::string name);
  bool addFile(std::string filename, bool datapath = false);
  bool addFile(HTTPFile file);
  void print();
  std::string& getValue();
  void operator=(const std::string val);  /* assign a value */
  
 public:
  HTTPParameterType type;
  std::string name;
  std::string value;
  std::vector<HTTPFile> files;              /* is filled in case of a file element (can contain multiple files) */
};

inline void HTTPParameter::setName(std::string n) {
  name = n;
}

inline void HTTPParameter::setType(HTTPParameterType t) {
  type = t;
}

inline size_t HTTPParameter::getNumFiles() {
  return files.size();
}

inline void HTTPParameter::operator=(const std::string val) {
  value = val;
}

inline std::string& HTTPParameter::getValue() {
  return value;
}

#endif
