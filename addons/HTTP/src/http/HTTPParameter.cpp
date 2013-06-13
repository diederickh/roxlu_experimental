#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <http/HTTPParameter.h>

HTTPParameter::HTTPParameter(HTTPParameterType type) 
  :type(type)
{
}

HTTPParameter::HTTPParameter(const HTTPParameter& o) {
  copy(o);
}

HTTPParameter& HTTPParameter::operator=(const HTTPParameter& o) {
  copy(o);
  return *this;
}

HTTPParameter& HTTPParameter::copy(const HTTPParameter& o) {
  name = o.name;
  value = o.value;
  type = o.type;
  std::copy(o.files.begin(), o.files.end(), std::back_inserter(files));
  return *this;
}

HTTPParameter::~HTTPParameter() {
}

void HTTPParameter::print() {
  if(type == HTTP_PARAMETER_NAME_VALUE) {
    RX_VERBOSE("%s = %s", name.c_str(), value.c_str());
  }
  else {
    RX_VERBOSE("%s = ... [unsupported parameter type (%d)]", name.c_str(), type);
  }
}

bool HTTPParameter::addFile(std::string filename, bool datapath) {
  HTTPFile f(filename, datapath);;
  return addFile(f);
}

bool HTTPParameter::addFile(HTTPFile file) {
  if(!file.exists()) {
    RX_ERROR("Cannot add the file because it wasn't found: `%s`", file.filepath.c_str());
    return false;
  }

  type = HTTP_PARAMETER_FILE;
  files.push_back(file);

  return true;
}
