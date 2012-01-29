#include "HE_HalfEdge.h"
//#include "HE_Vertex.h"

namespace roxlu {

HE_HalfEdge::HE_HalfEdge() 
	:vertex(NULL)
	,next(NULL)
	,prev(NULL)
	,pair(NULL)
	,face(NULL)
	,edge(NULL)
	
{
}

HE_HalfEdge::HE_HalfEdge(HE_Vertex* v)
	:vertex(v)
	,next(NULL)
	,prev(NULL)
	,pair(NULL)
	,face(NULL)
	,edge(NULL)
{
}

}; // roxlu