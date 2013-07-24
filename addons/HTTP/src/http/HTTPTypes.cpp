#include <roxlu/core/Utils.h>
#include <http/HTTPTypes.h>

HTTPFile::HTTPFile() {
  setDefaults();
}

HTTPFile::HTTPFile(std::string fname, bool datapath) {
  setDefaults();
  setFile(fname, datapath);
}

bool HTTPFile::setFile(std::string fname, bool datapath) {
  filepath = fname;
  filename = fname;
  if(datapath) {
    filepath = rx_to_data_path(fname);
  }
  return exists();
}

bool HTTPFile::exists() {
  return rx_file_exists(filepath);
}


void HTTPFile::setDefaults() {
  content_type = "application/octet-stream";
  transfer_encoding = "binary";
}
