#include "HE_Vertex.h"
#include "HE_HalfEdge.h"
//#include "ofMain.h" 

namespace roxlu {

HE_Vertex::HE_Vertex() 
	:label(-1)
	,half_edge(NULL)
{
}

HE_Vertex::HE_Vertex(Vec3 pos)
	:position(pos)
	,label(-1)
	,half_edge(NULL)
{
}

HE_Vertex::HE_Vertex(float nx, float ny, float nz)
	:label(-1)
	,half_edge(NULL)
{
	position.set(nx, ny, nz);
}

HE_Vertex::~HE_Vertex() {
	//cout << "~HE_Vertex" << endl;
}



set<HE_Vertex*> HE_Vertex::getNeighborVertices() {
	set<HE_Vertex*> result;
	if(getHalfEdge() == NULL) {
		return result;
	}
	HE_HalfEdge* he = half_edge;
	do {
		HE_HalfEdge* hen = he->getNext();

		if( (hen->getVertex() != this) 
			&& (result.find(hen->getVertex()) == result.end())
		)
		{
			//cout << "neigbor: " << hen->getVertex() << endl;
			result.insert(hen->getVertex());
		}
		he = he->getNextInVertex();
	} while(he != half_edge);
	//cout << "-----------------" << endl;
	return result;
}

}; // roxlu
