#ifndef ROXLU_HTTP_PARAMETERS_H
#define ROXLU_HTTP_PARAMETERS_H

#include <algorithm>
#include <map>
#include <string>
#include <sstream>

extern "C" {
#  include <uv.h>
}

#include <roxlu/core/Log.h>
#include <crypto/PercentEncode.h>
#include <http/HTTPParameter.h>

//-------------------------------------------------------------------------------

// find a parameter by name
struct HTTPParameterByName {
  HTTPParameterByName(std::string n):n(n){}
  bool operator()(const HTTPParameter& p) {
    return p.name == n;
  }
  std::string n;
};

//-------------------------------------------------------------------------------

// used to sort the parameters by name+value (used by oauth)
struct HTTPParameterSort {
  bool operator()(const HTTPParameter& a, const HTTPParameter& b) {
    return (a.name +a.value) < (b.name +b.value);
  }
};

//-------------------------------------------------------------------------------

class HTTPParameters {

 public:
  HTTPParameters();
  void add(HTTPParameter p);                                                           /* append the given parameter */
  void add(std::string name, std::string value);                                       /* add a simple name=value parameter */
  void copy(HTTPParameters& other);                                                    /* copy all parameters from the given collection */
  void copy(HTTPParameters& other, HTTPParameterType copyType);                        /* copy only parameters of the given type */
  void clear();                                                                        /* remove all parameters */
  bool hasFileParameter();                                                             /* check if we have a file parameters; this is used to set the correct form parameter */
  bool exist(std::string name);                                                        /* check if the given parameter exists */
  void print();                                                                        /* show some debugging information */
  void percentEncode();                                                                /* percent encode the name and value pairs */
  size_t createFromVariableString(std::string str);                                    /* parse a variable string into nam=value pair parameters */
  size_t size();                                                                       /* return the number of entries that this collection contains */
  std::string getQueryString();                                                        /* create a name=value&name=value string */
  bool toBoundaryString(std::string boundary, std::string& result);                    /* create a string that can be used in a post */
  bool toBoundaryString(HTTPParameter& p, std::string boundary, std::string& result);  /* append the parameter to the result for a multipart form post */
  HTTPParameter& operator[](const std::string name);                                   /* return the element by reference, when the entry isn't found it's created on the fly */

 public:
  std::map<std::string, HTTPParameter> entries;
};

//-------------------------------------------------------------------------------

inline void HTTPParameters::add(std::string name, std::string val) {
  add(HTTPParameter(name, val));
}

inline void HTTPParameters::add(HTTPParameter p) {
  entries.insert(std::pair<std::string, HTTPParameter>(p.name, p));
}

inline void HTTPParameters::copy(HTTPParameters& other) {
  entries.insert(other.entries.begin(), other.entries.end());
}

inline void HTTPParameters::clear() {
  entries.clear();
}

inline size_t HTTPParameters::size() {
  return entries.size();
}

inline bool HTTPParameters::hasFileParameter() {
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPParameter& p = it->second; 
    if(p.type == HTTP_PARAMETER_FILE) {
      return true;
    }
  }
  return false;
}

inline HTTPParameter& HTTPParameters::operator[](const std::string name) {
  HTTPParameter& p = entries[name];
  p.setName(name);
  return p;
}

#endif
