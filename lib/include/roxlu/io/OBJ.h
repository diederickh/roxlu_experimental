#ifndef ROXLU_OBJH
#define ROXLU_OBJH

#include <sstream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h> // atoi
#include <string.h> // memcpy
/** 
 * Super basic OBJ file importer. Only support the minimum to 
 * import one object, with the position and normal.
 */

namespace roxlu {
  
  class OBJ {
  public:
    struct TRI { int v, t, n; };
    struct FACE { TRI a, b, c; };
    struct XYZ {  float x, y, z; };

    bool load(const std::string& filepath);
    
    // we allocate memory for you but the caller is responsible for freeing
    size_t getVertices(float** result, bool normals = true);

  public:
    std::vector<OBJ::XYZ> vertices;
    std::vector<OBJ::XYZ> normals;
    std::vector<OBJ::FACE> faces;
    std::vector<int> indices;
  };

 
} // roxlu

#endif
