#ifndef ROXLU_STRING
#define ROXLU_STRING

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using std::string;
using std::vector;

// see pystring

namespace roxlu {

  class StringParts {
  public:
    StringParts();
    ~StringParts();
    string join(string sep);
    StringParts& pop();
    void addPart(string part);	
    size_t size();
    string& operator[](const unsigned int dx);
  private:
    vector<string> parts;
  };

  inline void StringParts::addPart(string part) {
    parts.push_back(part);
  }

  inline size_t StringParts::size() {
    return parts.size();
  }

  inline string& StringParts::operator[](const unsigned int dx) {
    return parts.at(dx);
  }

  class StringUtil {
  public:
    StringUtil();
    StringUtil(string data);
    ~StringUtil();
	
    inline StringUtil& set(string data);
    StringParts split(const char delim);
    StringUtil& operator=(const string data);
    string str();
	
    //static string stringFromFile(string path);

    // trim
    // -------------------------------------
    inline std::string trimLeft(std::string& s, const std::string& t = "\t\r\n") {
      std::string d(s);
      return d.erase(0, s.find_first_not_of(t));
    }
	
    inline std::string trimRight(std::string& s, const std::string& t = "\t\r\n") {	
      std::string d(s);
      std::string::size_type i(d.find_last_not_of(t));
      if(i == std::string::npos) {
        return "";
      }
      else {
        return d.erase(d.find_last_not_of(t)+1);
      }
    }

    inline std::string trim(std::string& s, const std::string& t = "\t\r\n") {
      std::string d(s);
      d = trimRight(d,t);
      return trimLeft(d,t);
    }
	
	
    // replace
    inline std::string stringReplace(std::string& src, std::string search, std::string replacement) {
      const std::string::size_type pos_in_src = src.find(search);
      if(pos_in_src != std::string::npos) {
        src.replace(pos_in_src, search.length(), replacement);
      }
      return trim(src);
    }
  private:
    string value;
  };


  inline StringUtil& StringUtil::set(string data) {
    value = data;
    return *this;
  }



}
#endif
