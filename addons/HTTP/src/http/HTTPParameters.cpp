#include <roxlu/core/Utils.h>
#include <http/HTTPParameters.h>

HTTPParameters::HTTPParameters() {
}

// create a string that can be used for a simple url encoded form post
std::string HTTPParameters::getQueryString() {
  std::string qs;
  size_t len = entries.size();
  size_t i = 0;
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPParameter& p = it->second;
    qs += p.name +"=" +p.value;
    if(i < len-1) {
      qs += "&";
    }
    ++i;
  }
  return qs;
}

// creates a string that can be used for a multi-part form post with boundaries
bool HTTPParameters::toBoundaryString(std::string boundary, std::string& result) {
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPParameter& p = it->second;
    if(!toBoundaryString(p, boundary, result)) {
      RX_ERROR("Cannot add parameter to post: %s", p.name.c_str());
      continue;
    }
  }
  result += "--" +boundary +"--\r\n";
  return true;
}

// creates the form content when using multipart/form-data
bool HTTPParameters::toBoundaryString(HTTPParameter& p, std::string boundary, std::string& result) {
  PercentEncode enc;
  std::string name = p.name; // enc.encode(p.name);
  std::string value = enc.encode(p.value);

  switch(p.type) {

    case HTTP_PARAMETER_NAME_VALUE: {
      result += "--" +boundary +"\r\n";
      result += "Content-Disposition: form-data; name=\"" +name +"\"\r\n";
      result += "\r\n";
      result += value +"\r\n";
      return true;
    }

    case HTTP_PARAMETER_FILE: {

      for(std::vector<HTTPFile>::iterator fit = p.files.begin(); fit != p.files.end(); ++fit) {
        HTTPFile& file = *fit;
        std::string file_contents = rx_get_file_contents(file.filepath, false);
        result += "--" +boundary +"\r\n";
        result += "Content-Type: " +file.content_type +"; name=\"" +name +"\"\r\n";
        result += "Content-Transfer-Encoding: " +file.transfer_encoding +"\r\n";
        result += "Content-Disposition: form-data; name=\"" +name +"\"; filename=\"" +file.filename +"\"\r\n";
        result += "\r\n";
        result += file_contents +"\r\n";
      }
      return true; 
    }

    default: {
      return false; 
    }

  }
}

// percent encodes name/value pairs 
void HTTPParameters::percentEncode() {
  PercentEncode enc;
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPParameter& p = it->second;
    if(p.type == HTTP_PARAMETER_NAME_VALUE) {
      p.name = enc.encode(p.name);
      p.value = enc.encode(p.value);
    }
    else {
      RX_VERBOSE("Unhandled percent encode for: %s", p.name.c_str());
    }
  }
}

size_t HTTPParameters::createFromVariableString(std::string str) {
  int s = 0;
  HTTPParameter p;
  for(int i = 0; i < str.size(); ++i) {
    if(str[i] == '=') {
      s = 1;
      continue;
    }
    else if(str[i] == '&') {
      add(p);
      p.name.clear();
      p.value.clear();
      s = 0;
      continue;
    }
    if(s == 0) {
      p.name.push_back(str[i]);
    }
    else if(s == 1) {
      p.value.push_back(str[i]);
    }
  }
  if(p.name.size()) {
    add(p);
  }
  return entries.size();
}

// check if the given parameter exist or not
bool HTTPParameters::exist(std::string n) {
  std::map<std::string, HTTPParameter>::iterator it = entries.find(n);
  if(it == entries.end()) {
    return false;
  }
  return true;
}

void HTTPParameters::print() {
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPParameter& p = it->second; 
    p.print();
  }
}

// copy only the given type
void HTTPParameters::copy(HTTPParameters& other, HTTPParameterType copyType) {
  for(std::map<std::string, HTTPParameter>::iterator it = other.entries.begin(); it != other.entries.end(); ++it) {
    HTTPParameter& p = it->second;
    if(p.type == copyType) {
      add(p);
    }
  }
}
