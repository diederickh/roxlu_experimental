#include <twitter/TwitterTypes.h>

//-------------------------------------------------------------------------------
TwitterCallParams::TwitterCallParams()
  :cb_user(NULL)
  ,cb_response(NULL)
  ,is_secure(false)
  ,method(HTTP_METHOD_GET)
{
}

bool TwitterCallParams::addFile(std::string name, std::string filename, bool datapath) {
  content_parameters[name].setName(name);
  
  HTTPFile f(filename, datapath);
  if(!content_parameters[name].addFile(f)) {
    RX_ERROR("Error while adding a file");
    return false;
  }

  return true;
}

//-------------------------------------------------------------------------------

TwitterStatusesUpdate::TwitterStatusesUpdate(std::string status) {
  query_parameters.add("status", status);
}

bool TwitterStatusesUpdate::addMedia(std::string file, bool datapath) {
  addFile("media[]", file, datapath);
  return false;
}

//-------------------------------------------------------------------------------
void TwitterStatusesFilter::track(std::string tr) {
  query_parameters["track"] = tr;
}

void TwitterStatusesFilter::follow(std::string fol) {
  query_parameters["follow"] = fol;
}

