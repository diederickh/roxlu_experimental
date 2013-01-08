#include <roxlu/io/OBJ.h>

namespace roxlu {

  bool OBJ::load(const std::string& filepath) {
    std::ifstream ifs;
    ifs.open(filepath.c_str());
    if(!ifs.is_open()) {
      printf("ERROR: Cannot open: %s\n", filepath.c_str());
      return false;
    }	
    char c;
    std::string line;
    while(std::getline(ifs, line)) {
      std::stringstream ss(line);
      ss >> c;
      if(c == 'v') {
        if(line[1] == ' ') {
          XYZ p;
          ss >> p.x >> p.y >> p.z;
          vertices.push_back(p);
        }
        else if(line[1] == 'n') {
          XYZ p;
          ss >> c;
          ss >> p.x >> p.y >> p.z;
          normals.push_back(p);
        }
      }
      else if(c == 'f') {
        std::string part;
        std::vector<OBJ::TRI> tris;
        while(ss >> part) {
          std::stringstream fss;
          std::string indices[3];
          int dx = 0;
          for(int i = 0; i < part.size(); ++i) {
            if(part[i] == '/') {
              dx++;
              continue;
            }
            indices[dx].push_back(part[i]);
          }
          TRI tri;
          tri.v = atoi(indices[0].c_str()) - 1;
          tri.t = atoi(indices[1].c_str()) - 1;
          tri.n = atoi(indices[2].c_str()) - 1;
          tris.push_back(tri);
        }
        if(tris.size() == 3) {
          OBJ::FACE face;
          face.a = tris[0];
          face.b = tris[1];
          face.c = tris[2];
          faces.push_back(face);
        }
        else {
          printf("ERROR: incorrect number of indices in face.\n");
        }
      }
    }
    return true;
  }

#define OBJ_VERTEX_NORMAL(tri) {                                        \
    memcpy(ptr+dx, (char*)&vertices[tri.v].x, sizeof(XYZ));  dx += 3;   \
    memcpy(ptr+dx, (char*)&normals[tri.n].x, sizeof(XYZ));  dx += 3;    \
  }

#define OBJ_VERTEX(tri) {                                               \
    memcpy(ptr+dx, (char*)&vertices[tri.v].x, sizeof(XYZ));  dx += 3;   \
  }

  // returns number of vertices
  size_t OBJ::getVertices(float** result, bool useNormals) {
    int num_xyz = 1; // only vertices;
    if(useNormals) {  // also normals
      num_xyz++;
    }
    size_t num_floats_per_vertex = num_xyz * 3;
    size_t num_vertices = faces.size() * 3;
    size_t num_floats = num_vertices * num_floats_per_vertex;
    float* ptr = new float[num_floats];
    size_t dx = 0;
    OBJ::TRI tri;
    OBJ::XYZ vertex;
    OBJ::XYZ normal;
    for(std::vector<FACE>::iterator it = faces.begin(); it != faces.end(); ++it) {
      FACE& f = *it;
      if(useNormals) {
        OBJ_VERTEX_NORMAL(f.a);
        OBJ_VERTEX_NORMAL(f.b);
        OBJ_VERTEX_NORMAL(f.c);
      }
      else {
        OBJ_VERTEX(f.a);
        OBJ_VERTEX(f.b);
        OBJ_VERTEX(f.c);
      }
    }
    *result = ptr;
    return num_vertices;
  }
};


