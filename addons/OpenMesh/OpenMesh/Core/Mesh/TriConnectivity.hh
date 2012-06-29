/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         * 
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *             
 *   $Revision: 456 $                                                         *
 *   $Date: 2011-11-14 09:16:07 +0100 (Mo, 14 Nov 2011) $                   *
 *                                                                           *
\*===========================================================================*/

#ifndef OPENMESH_TRICONNECTIVITY_HH
#define OPENMESH_TRICONNECTIVITY_HH

#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>

namespace OpenMesh {

/** \brief Connectivity Class for Triangle Meshes
*/
class TriConnectivity : public PolyConnectivity
{
public:

  TriConnectivity() {}
  virtual ~TriConnectivity() {}

  inline static bool is_triangles()
  { return true; }

  /** assign_connectivity() methods. See ArrayKernel::assign_connectivity()
      for more details. When the source connectivity is not triangles, in
      addition "fan" connectivity triangulation is performed*/
  inline void assign_connectivity(const TriConnectivity& _other)
  { PolyConnectivity::assign_connectivity(_other); }
  
  inline void assign_connectivity(const PolyConnectivity& _other)
  { 
    PolyConnectivity::assign_connectivity(_other); 
    triangulate();
  }
  
  /** \name Addding items to a mesh
  */
  //@{
  /** Override OpenMesh::Mesh::PolyMeshT::add_face(). Faces that aren't
      triangles will be triangulated and added. In this case an
      invalid face handle will be returned.  */
  FaceHandle add_face(const std::vector<VertexHandle>& _vhandles)
  { return add_face(&_vhandles.front(), _vhandles.size()); }
  
  FaceHandle add_face(const VertexHandle* _vhandles, uint _vhs_size);
  
  FaceHandle add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2)
  { 
    VertexHandle vhs[3] = { _vh0, _vh1, _vh2 };
    return PolyConnectivity::add_face(vhs, 3); 
  }
  
  //@}

  /** Returns the opposite vertex to the halfedge _heh in the face
      referenced by _heh returns InvalidVertexHandle if the _heh is
      boundary  */
  inline VertexHandle opposite_vh(HalfedgeHandle _heh) const
  {
    return is_boundary(_heh) ? InvalidVertexHandle :
                               to_vertex_handle(next_halfedge_handle(_heh));
  }

  /** Returns the opposite vertex to the opposite halfedge of _heh in
      the face referenced by it returns InvalidVertexHandle if the
      opposite halfedge is boundary  */
  VertexHandle opposite_he_opposite_vh(HalfedgeHandle _heh) const
  { return opposite_vh(opposite_halfedge_handle(_heh)); }

  /** \name Topology modifying operators
  */
  //@{


  /** Returns whether collapsing halfedge _heh is ok or would lead to
      topological inconsistencies.
      \attention This method need the Attributes::Status attribute and
      changes the \em tagged bit.  */
  bool is_collapse_ok(HalfedgeHandle _heh);

  /// Vertex Split: inverse operation to collapse().
  HalfedgeHandle vertex_split(VertexHandle v0, VertexHandle v1,
                              VertexHandle vl, VertexHandle vr);

  /// Check whether flipping _eh is topologically correct.
  bool is_flip_ok(EdgeHandle _eh) const;

  /** Flip edge _eh.
      Check for topological correctness first using is_flip_ok(). */
  void flip(EdgeHandle _eh);

  /// Edge split (= 2-to-4 split)
  void split(EdgeHandle _eh, VertexHandle _vh);

  /// Face split (= 1-to-3 split, calls corresponding PolyMeshT function).
  inline void split(FaceHandle _fh, VertexHandle _vh)
  { PolyConnectivity::split(_fh, _vh); }

  //@}

private:
  /// Helper for vertex split
  HalfedgeHandle insert_loop(HalfedgeHandle _hh);
  /// Helper for vertex split
  HalfedgeHandle insert_edge(VertexHandle _vh,
                             HalfedgeHandle _h0, HalfedgeHandle _h1);
};

}

#endif//OPENMESH_TRICONNECTIVITY_HH
