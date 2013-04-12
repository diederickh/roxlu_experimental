/* 
  
   OBJ
   ---
  Super basic OBJ file importer. Only support the minimum to 
  import one object, with the position and normal.
 
*/


#ifndef ROXLU_OPENGL_OBJ_H
#define ROXLU_OPENGL_OBJ_H

#include <sstream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <string.h>                        /* memcpy/memset */
#include <stdlib.h>                        /* atoi */
#include <glr/Vertex.h>
#include <glr/VBO.h>

#define ERR_OBJ_FILE_NOT_FOUND "Could not open the file: `%s`"
#define ERR_OBJ_WRONG_FACE_INDICES "Incorrect number of indices in face: %ld"
#define ERR_OBJ_NO_VERTICES "No vertices loaded from OBJ file."
#define ERR_OBJ_NO_NORMALS "No normals loaded from OBJ file."

namespace gl {

class OBJ {
 public:
  struct TRI { int v, t, n; };
  struct FACE { TRI a, b, c; };
  struct XYZ {  float x, y, z; };
  struct TEXCOORD { float s, t; };

  bool load(std::string filepath, bool datapath = false);
    
  bool copyVertices(VBO<VertexP>& vbo);
  bool copyVertices(VBO<VertexNP>& vbo);

 public:
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec3> tex_coords;
  std::vector<OBJ::FACE> faces;
  std::vector<int> indices;
};

} // gl
#endif
