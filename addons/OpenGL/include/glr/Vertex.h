#ifndef ROXLU_OPENGL_VERTEX_H
#define ROXLU_OPENGL_VERTEX_H

#include <roxlu/Roxlu.h>

namespace gl {

  struct VertexP {
    VertexP();
    VertexP(Vec3 pos);

    Vec3 pos;
  };

  struct VertexCP {
    VertexCP();
    VertexCP(Vec4 color, Vec3 pos);

    Vec4 color;
    Vec3 pos;
  };

  struct VertexPT {
    VertexPT();
    VertexPT(Vec3 pos, Vec2 tex);

    Vec3 pos;
    Vec2 tex;
  };

  struct VertexNP {
    VertexNP();
    VertexNP(Vec3 norm, Vec3 pos);

   Vec3 norm; 
   Vec3 pos;
 
  };

  struct VertexNPT {
    VertexNPT();
    VertexNPT(Vec3 pos, Vec3 norm, Vec3 tex);

    Vec3 norm;
    Vec3 pos;
    Vec2 tex;
  };

  // ----------------------------------

  inline VertexP::VertexP() {
  }
  
  inline VertexP::VertexP(Vec3 pos)
    :pos(pos) 
  {
  }

  inline VertexCP::VertexCP() {
  }
  
  inline VertexCP::VertexCP(Vec4 color, Vec3 pos)
    :color(color)
    ,pos(pos)
  {
  }

  inline VertexPT::VertexPT() {
  }

  inline VertexPT::VertexPT(Vec3 pos, Vec2 tex)
    :pos(pos)
    ,tex(tex) 
  {
  }

  inline VertexNP::VertexNP() {
  }

  inline VertexNP::VertexNP(Vec3 norm, Vec3 pos)
    :norm(norm) 
    ,pos(pos)
  {
  }

  inline VertexNPT::VertexNPT() {
  }
  
  inline VertexNPT::VertexNPT(Vec3 norm, Vec3 pos, Vec3 tex) 
    :pos(pos)
    ,norm(norm)
    ,tex(tex)
  {
  }

} // gl

#endif
