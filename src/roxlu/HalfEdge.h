#include "HalfEdge/HE_Edge.h"
#include "HalfEdge/HE_Face.h"
#include "HalfEdge/HE_HalfEdge.h"
#include "HalfEdge/HE_Mesh.h"
#include "HalfEdge/HE_Selection.h"
#include "HalfEdge/HE_Structure.h"
#include "HalfEdge/HE_Vertex.h"
#include "HalfEdge/HEC_Creator.h"
#include "HalfEdge/HEC_FromFaceList.h"
#include "HalfEdge/HEC_FromTriangles.h"
#include "HalfEdge/HEC_Plane.h"
#include "HalfEdge/HEM_Bend.h"
#include "HalfEdge/HEM_Modifier.h"
#include "HalfEdge/HEM_Noise.h"
#include "HalfEdge/HES_CatmullClark.h"
#include "HalfEdge/HES_Subdividor.h"

// @todo http://openmesh.org/Documentation/OpenMesh-2.0-Documentation/index.html
// @todo Check: http://vefur.simula.no/ogl/ttl/halfedge_doc/html/hesimplest.html