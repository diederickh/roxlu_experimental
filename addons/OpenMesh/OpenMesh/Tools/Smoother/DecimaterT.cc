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
 *   $Revision: 460 $                                                         *
 *   $Date: 2011-11-16 10:45:08 +0100 (Mi, 16 Nov 2011) $                   *
 *                                                                           *
 \*===========================================================================*/

/** \file DecimaterT.cc
 */

//=============================================================================
//
//  CLASS DecimaterT - IMPLEMENTATION
//
//=============================================================================
#define OPENMESH_DECIMATER_DECIMATERT_CC

//== INCLUDES =================================================================

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>

#include <vector>
#if defined(OM_CC_MIPS)
#  include <float.h>
#else
#  include <cfloat>
#endif

//== NAMESPACE ===============================================================

namespace OpenMesh {
namespace Decimater {

//== IMPLEMENTATION ==========================================================

template<class Mesh>
DecimaterT<Mesh>::DecimaterT(Mesh& _mesh) :
    mesh_(_mesh), heap_(NULL), cmodule_(NULL), initialized_(false) {
  // default properties
  mesh_.request_vertex_status();
  mesh_.request_edge_status();
  mesh_.request_face_status();
  mesh_.request_face_normals();

  // private vertex properties
  mesh_.add_property(collapse_target_);
  mesh_.add_property(priority_);
  mesh_.add_property(heap_position_);
}

//-----------------------------------------------------------------------------

template<class Mesh>
DecimaterT<Mesh>::~DecimaterT() {
  // default properties
  mesh_.release_vertex_status();
  mesh_.release_edge_status();
  mesh_.release_face_status();
  mesh_.release_face_normals();

  // private vertex properties
  mesh_.remove_property(collapse_target_);
  mesh_.remove_property(priority_);
  mesh_.remove_property(heap_position_);

  // dispose of modules
  {
    set_uninitialized();
    typename ModuleList::iterator m_it, m_end = all_modules_.end();
    for (m_it = all_modules_.begin(); m_it != m_end; ++m_it)
      delete *m_it;
    all_modules_.clear();
  }
}

//-----------------------------------------------------------------------------

template<class Mesh>
void DecimaterT<Mesh>::info(std::ostream& _os) {
  if (initialized_) {
    _os << "initialized : yes" << std::endl;
    _os << "binary modules: " << bmodules_.size() << std::endl;
    for (ModuleListIterator m_it = bmodules_.begin(); m_it != bmodules_.end();
        ++m_it) {
      _os << "  " << (*m_it)->name() << std::endl;
    }
    _os << "priority module: " << cmodule_->name().c_str() << std::endl;
  } else {
    _os << "initialized : no" << std::endl;
    _os << "available modules: " << all_modules_.size() << std::endl;
    for (ModuleListIterator m_it = all_modules_.begin();
        m_it != all_modules_.end(); ++m_it) {
      _os << "  " << (*m_it)->name() << " : ";
      if ((*m_it)->is_binary()) {
        _os << "binary";
        if ((*m_it)->name() == "Quadric") {
          _os << " and priority (special treatment)";
        }
      } else {
        _os << "priority";
      }
      _os << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

template<class Mesh>
bool DecimaterT<Mesh>::initialize() {
  if (initialized_) {
    return true;
  }

  // FIXME: quadric module shouldn't be treated specially.
  // Q: Why?
  // A: It isn't generic and breaks encapsulation. Also, using string
  // name comparison is not reliable, since you can't guarantee that
  // no one else will name their custom module "Quadric".
  // Q: What should be done instead?
  // A: ModBaseT API should support modules that can be both binary
  // and priority, or BETTER YET, let the DecimaterT API specify the
  // priority module explicitly.

  // find the priority module: either the only non-binary module in the list, or "Quadric"
  Module *quadric = NULL;
  Module *pmodule = NULL;
  for (ModuleListIterator m_it = all_modules_.begin(), m_end =
      all_modules_.end(); m_it != m_end; ++m_it) {
    if ((*m_it)->name() == "Quadric")
      quadric = *m_it;

    if (!(*m_it)->is_binary()) {
      if (pmodule) {
        // only one priority module allowed!
        set_uninitialized();
        return false;
      }
      pmodule = *m_it;
    }
  }

  // Quadric is used as default priority module (even if it is set to be binary)
  if (!pmodule && quadric) {
    pmodule = quadric;
  }

  if (!pmodule) {
    // At least one priority module required
    set_uninitialized();
    return false;
  }

  // set pmodule as the current priority module
  cmodule_ = pmodule;

  for (ModuleListIterator m_it = all_modules_.begin(), m_end =
      all_modules_.end(); m_it != m_end; ++m_it) {
    // every module gets initialized
    (*m_it)->initialize();

    if (*m_it != pmodule) {
      // all other modules are binary, and go into bmodules_ list
      bmodules_.push_back(*m_it);
    }
  }

  return initialized_ = true;
}

//-----------------------------------------------------------------------------

template<class Mesh>
bool DecimaterT<Mesh>::is_collapse_legal(const CollapseInfo& _ci) {
  //   std::clog << "DecimaterT<>::is_collapse_legal()\n";

  // locked ? deleted ?
  if (mesh_.status(_ci.v0).locked() || mesh_.status(_ci.v0).deleted())
    return false;
  /*
   if (!mesh_.is_collapse_ok(_ci.v0v1))
   {
   return false;
   }
   */
  if (_ci.vl.is_valid() && _ci.vr.is_valid()
      && mesh_.find_halfedge(_ci.vl, _ci.vr).is_valid()
      && mesh_.valence(_ci.vl) == 3 && mesh_.valence(_ci.vr) == 3) {
    return false;
  }
  //--- feature test ---

  if (mesh_.status(_ci.v0).feature()
      && !mesh_.status(mesh_.edge_handle(_ci.v0v1)).feature())
    return false;

  //--- test one ring intersection ---

  typename Mesh::VertexVertexIter vv_it;

  for (vv_it = mesh_.vv_iter(_ci.v0); vv_it; ++vv_it)
    mesh_.status(vv_it).set_tagged(false);

  for (vv_it = mesh_.vv_iter(_ci.v1); vv_it; ++vv_it)
    mesh_.status(vv_it).set_tagged(true);

  for (vv_it = mesh_.vv_iter(_ci.v0); vv_it; ++vv_it)
    if (mesh_.status(vv_it).tagged() && vv_it.handle() != _ci.vl
        && vv_it.handle() != _ci.vr)
      return false;

  // if both are invalid OR equal -> fail
  if (_ci.vl == _ci.vr)
    return false;

  //--- test boundary cases ---
  if (mesh_.is_boundary(_ci.v0)) {
    if (!mesh_.is_boundary(_ci.v1)) { // don't collapse a boundary vertex to an inner one
      return false;
    } else { // edge between two boundary vertices has to be a boundary edge
      if (!(mesh_.is_boundary(_ci.v0v1) || mesh_.is_boundary(_ci.v1v0)))
        return false;
    }
    // only one one ring intersection
    if (_ci.vl.is_valid() && _ci.vr.is_valid())
      return false;
  }

  // v0vl and v1vl must not both be boundary edges
  if (_ci.vl.is_valid() && mesh_.is_boundary(_ci.vlv1)
      && mesh_.is_boundary(_ci.v0vl))
    return false;

  // v0vr and v1vr must not be both boundary edges
  if (_ci.vr.is_valid() && mesh_.is_boundary(_ci.vrv0)
      && mesh_.is_boundary(_ci.v1vr))
    return false;

  // there have to be at least 2 incident faces at v0
  if (mesh_.cw_rotated_halfedge_handle(
      mesh_.cw_rotated_halfedge_handle(_ci.v0v1)) == _ci.v0v1)
    return false;

  // collapse passed all tests -> ok
  return true;
}

//-----------------------------------------------------------------------------

template<class Mesh>
float DecimaterT<Mesh>::collapse_priority(const CollapseInfo& _ci) {
  typename ModuleList::iterator m_it, m_end = bmodules_.end();

  for (m_it = bmodules_.begin(); m_it != m_end; ++m_it) {
    if ((*m_it)->collapse_priority(_ci) < 0.0)
      return ModBaseT<DecimaterT<Mesh> >::ILLEGAL_COLLAPSE;
  }
  return cmodule_->collapse_priority(_ci);
}

//-----------------------------------------------------------------------------

template<class Mesh>
void DecimaterT<Mesh>::heap_vertex(VertexHandle _vh) {
  //   std::clog << "heap_vertex: " << _vh << std::endl;

  float prio, best_prio(FLT_MAX);
  typename Mesh::HalfedgeHandle heh, collapse_target;

  // find best target in one ring
  typename Mesh::VertexOHalfedgeIter voh_it(mesh_, _vh);
  for (; voh_it; ++voh_it) {
    heh = voh_it.handle();
    CollapseInfo ci(mesh_, heh);

    if (is_collapse_legal(ci)) {
      prio = collapse_priority(ci);
      if (prio >= 0.0 && prio < best_prio) {
        best_prio = prio;
        collapse_target = heh;
      }
    }
  }

  // target found -> put vertex on heap
  if (collapse_target.is_valid()) {
    //     std::clog << "  added|updated" << std::endl;
    mesh_.property(collapse_target_, _vh) = collapse_target;
    mesh_.property(priority_, _vh) = best_prio;

    if (heap_->is_stored(_vh))
      heap_->update(_vh);
    else
      heap_->insert(_vh);
  }

  // not valid -> remove from heap
  else {
    //     std::clog << "  n/a|removed" << std::endl;
    if (heap_->is_stored(_vh))
      heap_->remove(_vh);

    mesh_.property(collapse_target_, _vh) = collapse_target;
    mesh_.property(priority_, _vh) = -1;
  }
}

//-----------------------------------------------------------------------------

template<class Mesh>
void DecimaterT<Mesh>::postprocess_collapse(CollapseInfo& _ci) {
  typename ModuleList::iterator m_it, m_end = bmodules_.end();

  for (m_it = bmodules_.begin(); m_it != m_end; ++m_it)
    (*m_it)->postprocess_collapse(_ci);

  cmodule_->postprocess_collapse(_ci);
}

//-----------------------------------------------------------------------------

template<class Mesh>
void DecimaterT<Mesh>::preprocess_collapse(CollapseInfo& _ci) {
  typename ModuleList::iterator m_it, m_end = bmodules_.end();

  for (m_it = bmodules_.begin(); m_it != m_end; ++m_it)
    (*m_it)->preprocess_collapse(_ci);

  cmodule_->preprocess_collapse(_ci);
}

//-----------------------------------------------------------------------------
template<class Mesh>
size_t DecimaterT<Mesh>::decimate(size_t _n_collapses) {
  if (!is_initialized())
    return 0;

  typename Mesh::VertexIter v_it, v_end(mesh_.vertices_end());
  typename Mesh::VertexHandle vp;
  typename Mesh::HalfedgeHandle v0v1;
  typename Mesh::VertexVertexIter vv_it;
  typename Mesh::VertexFaceIter vf_it;
  unsigned int n_collapses(0);

  typedef std::vector<typename Mesh::VertexHandle> Support;
  typedef typename Support::iterator SupportIterator;

  Support support(15);
  SupportIterator s_it, s_end;

  // check _n_collapses
  if (!_n_collapses)
    _n_collapses = mesh_.n_vertices();

  // initialize heap
  HeapInterface HI(mesh_, priority_, heap_position_);
  heap_ = std::auto_ptr<DeciHeap>(new DeciHeap(HI));
  heap_->reserve(mesh_.n_vertices());

  for (v_it = mesh_.vertices_begin(); v_it != v_end; ++v_it) {
    heap_->reset_heap_position(v_it.handle());
    if (!mesh_.status(v_it).deleted())
      heap_vertex(v_it.handle());
  }

  // process heap
  while ((!heap_->empty()) && (n_collapses < _n_collapses)) {
    // get 1st heap entry
    vp = heap_->front();
    v0v1 = mesh_.property(collapse_target_, vp);
    heap_->pop_front();

    // setup collapse info
    CollapseInfo ci(mesh_, v0v1);

    // check topological correctness AGAIN !
    if (!is_collapse_legal(ci))
      continue;

    // store support (= one ring of *vp)
    vv_it = mesh_.vv_iter(ci.v0);
    support.clear();
    for (; vv_it; ++vv_it)
      support.push_back(vv_it.handle());

    // perform collapse
    mesh_.collapse(v0v1);
    ++n_collapses;

    // update triangle normals
    vf_it = mesh_.vf_iter(ci.v1);
    for (; vf_it; ++vf_it)
      if (!mesh_.status(vf_it).deleted())
        mesh_.set_normal(vf_it, mesh_.calc_face_normal(vf_it.handle()));

    // post-process collapse
    postprocess_collapse(ci);

    // update heap (former one ring of decimated vertex)
    for (s_it = support.begin(), s_end = support.end(); s_it != s_end; ++s_it) {
      assert(!mesh_.status(*s_it).deleted());
      heap_vertex(*s_it);
    }
  }

  // delete heap
  heap_.reset();

  // DON'T do garbage collection here! It's up to the application.
  return n_collapses;
}

//-----------------------------------------------------------------------------

template<class Mesh>
size_t DecimaterT<Mesh>::decimate_to_faces(size_t _nv, size_t _nf) {
  if (!is_initialized())
    return 0;

  if (_nv >= mesh_.n_vertices() || _nf >= mesh_.n_faces())
    return 0;

  typename Mesh::VertexIter v_it, v_end(mesh_.vertices_end());
  typename Mesh::VertexHandle vp;
  typename Mesh::HalfedgeHandle v0v1;
  typename Mesh::VertexVertexIter vv_it;
  typename Mesh::VertexFaceIter vf_it;
  unsigned int nv = mesh_.n_vertices();
  unsigned int nf = mesh_.n_faces();
  unsigned int n_collapses = 0;

  typedef std::vector<typename Mesh::VertexHandle> Support;
  typedef typename Support::iterator SupportIterator;

  Support support(15);
  SupportIterator s_it, s_end;

  // initialize heap
  HeapInterface HI(mesh_, priority_, heap_position_);
  heap_ = std::auto_ptr<DeciHeap>(new DeciHeap(HI));
  heap_->reserve(mesh_.n_vertices());

  for (v_it = mesh_.vertices_begin(); v_it != v_end; ++v_it) {
    heap_->reset_heap_position(v_it.handle());
    if (!mesh_.status(v_it).deleted())
      heap_vertex(v_it.handle());
  }

  // process heap
  while ((!heap_->empty()) && (_nv < nv) && (_nf < nf)) {
    // get 1st heap entry
    vp = heap_->front();
    v0v1 = mesh_.property(collapse_target_, vp);
    heap_->pop_front();

    // setup collapse info
    CollapseInfo ci(mesh_, v0v1);

    // check topological correctness AGAIN !
    if (!is_collapse_legal(ci))
      continue;

    // store support (= one ring of *vp)
    vv_it = mesh_.vv_iter(ci.v0);
    support.clear();
    for (; vv_it; ++vv_it)
      support.push_back(vv_it.handle());

    // adjust complexity in advance (need boundary status)
    ++n_collapses;
    --nv;
    if (mesh_.is_boundary(ci.v0v1) || mesh_.is_boundary(ci.v1v0))
      --nf;
    else
      nf -= 2;

    // pre-processing
    preprocess_collapse(ci);

    // perform collapse
    mesh_.collapse(v0v1);

    // update triangle normals
    vf_it = mesh_.vf_iter(ci.v1);
    for (; vf_it; ++vf_it)
      if (!mesh_.status(vf_it).deleted())
        mesh_.set_normal(vf_it, mesh_.calc_face_normal(vf_it.handle()));

    // post-process collapse
    postprocess_collapse(ci);

    // update heap (former one ring of decimated vertex)
    for (s_it = support.begin(), s_end = support.end(); s_it != s_end; ++s_it) {
      assert(!mesh_.status(*s_it).deleted());
      heap_vertex(*s_it);
    }
  }

  // delete heap
  heap_.reset();

  // DON'T do garbage collection here! It's up to the application.
  return n_collapses;
}

//=============================================================================
}// END_NS_DECIMATER
} // END_NS_OPENMESH
//=============================================================================

