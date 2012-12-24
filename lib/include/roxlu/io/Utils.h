#ifndef ROXLU_IO_UTILS_H
#define ROXLU_IO_UTILS_H

#include <roxlu/io/File.h>
#include <roxlu/io/OBJ.h>

// Simple wrapper which loads an object file from the data path, returns number of vertices
static size_t rx_load_obj_file(const std::string& filename, float** result, bool useNormals) {
  roxlu::OBJ obj;
  if(!obj.load(roxlu::File::toDataPath(filename))) {
    *result = NULL;
    return false;
  }
  return obj.getVertices(result, useNormals);
}

#endif
