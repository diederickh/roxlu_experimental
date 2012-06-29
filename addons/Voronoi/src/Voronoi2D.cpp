#include "Voronoi2D.h"
namespace roxlu {

Voronoi2D::Voronoi2D()
	:vl(NULL)
	,con(NULL)
{
}

Voronoi2D::~Voronoi2D() {
	if(vl != NULL) {
		delete vl;
	}
	if(con != NULL) {
		delete con;
	}
}

void Voronoi2D::setup(
		 double minX			// min x of bounding box
		,double maxX			// max x of bounding box
		,double minY			// min y of bounding box
		,double maxY			// max y of bounding box
		,int numGridsX			// number of grid cells in x
		,int numGridsY			// number of grid cells in y
		,int numParticlesPerGridCell // how many particles to start per grid
)
{
	con = new voro::container_2d(minX, maxX, minY, maxY, numGridsX, numGridsY, false, false, numParticlesPerGridCell);
}



} // roxlu