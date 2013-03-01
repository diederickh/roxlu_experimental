#ifndef KURL_FORM_H
#define KURL_FORM_H

#include <vector>

struct FormInput {
  std::string name;
  std::string value;
};

struct FormFile {
  std::string name;
  std::string filepath;
};

class Form {
 public:
  Form() { }
  ~Form() {}
  void setURL(std::string u);
  void addInput(std::string name, std::string value);
  void addFile(std::string name, std::string filepath);
  bool isSetup();
 public:
  std::string url;
  std::vector<FormInput> inputs;
  std::vector<FormFile> files;

};

inline void Form::addInput(std::string name, std::string value) {
  FormInput fi; 
  fi.name = name;
  fi.value = value;
  inputs.push_back(fi);
}

inline void Form::addFile(std::string name, std::string filepath) {
  FormFile ff;
  ff.name = name;
  ff.filepath = filepath;
  files.push_back(ff);
}

inline void Form::setURL(std::string u) {
  url = u;
}

inline bool Form::isSetup() {
  if(!url.size()) {
    return false;
  }
  if(inputs.size() == 0 && files.size() == 0) {
    return false;
  }
  return true;
}
#endif
