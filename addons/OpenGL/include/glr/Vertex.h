#ifndef ROXLU_OPENGL_VERTEX_H
#define ROXLU_OPENGL_VERTEX_H

#include <roxlu/Roxlu.h>

namespace gl {


#define VERTEX_P     1        /* vertex with only a position */
#define VERTEX_PT    2        /* vertex with: position, texture coord */
#define VERTEX_PN    3        /* vertex with: position normal */
#define VERTEX_PNT   4        /* vertex with: position, normal, texture coord */
#define VERTEX_NONE 9999      /* undefined */


  struct VertexP {
    VertexP();
    VertexP(Vec3 pos);

    Vec3 pos;
  };

  struct VertexPT {
    VertexPT();
    VertexPT(Vec3 pos, Vec2 tex);

    Vec3 pos;
    Vec2 tex;
  };

  struct VertexPN {
    VertexPN();
    VertexPN(Vec3 pos, Vec3 norm);

    Vec3 pos;
    Vec3 norm;
  };

  struct VertexPNT {
    VertexPNT();
    VertexPNT(Vec3 pos, Vec3 norm, Vec3 tex);
    Vec3 pos;
    Vec3 norm;
    Vec2 tex;
  };


  // ----------------------------------

  inline VertexP::VertexP() {
  }
  
  inline VertexP::VertexP(Vec3 pos)
    :pos(pos) 
  {
  }

  inline VertexPT::VertexPT() {
  }

  inline VertexPT::VertexPT(Vec3 pos, Vec2 tex)
    :pos(pos)
    ,tex(tex) 
  {
  }

  inline VertexPN::VertexPN() {
  }

  inline VertexPN::VertexPN(Vec3 pos, Vec3 norm)
    :pos(pos)
    ,norm(norm) 
  {
  }

  inline VertexPNT::VertexPNT() {
  }
  
  inline VertexPNT::VertexPNT(Vec3 pos, Vec3 norm, Vec3 tex) 
    :pos(pos)
    ,norm(norm)
    ,tex(tex)
  {
  }

} // gl

#endif
