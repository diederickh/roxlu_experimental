#include <roxlu/core/Utils.h>
#include <io/OBJ.h>

namespace gl {

  bool OBJ::load(std::string filepath, bool datapath) {

    if(datapath) {
      filepath = rx_to_data_path(filepath);
    }

    std::ifstream ifs;
    ifs.open(filepath.c_str());
    if(!ifs.is_open()) {
      RX_ERROR(ERR_OBJ_FILE_NOT_FOUND, filepath.c_str());
      return false;
    }	

    char c;
    std::string line;
    while(std::getline(ifs, line)) {
      std::stringstream ss(line);
      ss >> c;
      if(c == 'v') {
        if(line[1] == ' ') {
          Vec3 p;
          ss >> p.x >> p.y >> p.z;
          vertices.push_back(p);
        }
        else if(line[1] == 'n') {
          Vec3 p;
          ss >> c;
          ss >> p.x >> p.y >> p.z;
          normals.push_back(p);
        }
        else if (line[1] == 't') {
          Vec2 t;
          ss >> c;
          ss >> t.x >> t.y;
          t.y = 1.0f - t.y;
          tex_coords.push_back(t);
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
          RX_ERROR(ERR_OBJ_WRONG_FACE_INDICES, tris.size());
        }
      }
    }
    return true;
  }

  bool OBJ::copyVertices(VBO<VertexP>& vbo) {

    if(!vertices.size()) {
      RX_ERROR(ERR_OBJ_NO_VERTICES);
      return false;
    }
  
    for(std::vector<FACE>::iterator it = faces.begin(); it != faces.end(); ++it) {
      FACE& f = *it;
      vbo.push_back(VertexP(vertices[f.a.v]));
      vbo.push_back(VertexP(vertices[f.b.v]));
      vbo.push_back(VertexP(vertices[f.c.v]));
    }

    return true;
  }

  bool OBJ::copyVertices(VBO<VertexNP>& vbo) {

    if(!vertices.size()) {
      RX_ERROR(ERR_OBJ_NO_VERTICES);
      return false;
    }

    if(!normals.size()) {
      RX_ERROR(ERR_OBJ_NO_NORMALS);
      return false;
    }

    for(std::vector<FACE>::iterator it = faces.begin(); it != faces.end(); ++it) {
      FACE& f = *it;
      vbo.push_back(VertexNP(normals[f.a.n], vertices[f.a.v]));
      vbo.push_back(VertexNP(normals[f.b.n], vertices[f.b.v]));
      vbo.push_back(VertexNP(normals[f.c.n], vertices[f.c.v]));
    }
    
    return true;
  }

} // gl

