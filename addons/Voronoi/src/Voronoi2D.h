#ifndef ROXLU_VORO2DH
#define ROXLU_VORO2DH

#include "../lib/config.hh"
#include "../lib/common.hh"
#include "../lib/cell_2d.hh"
#include "../lib/v_base_2d.hh"
#include "../lib/rad_option.hh"
#include "../lib/container_2d.hh"
#include "../lib/v_compute_2d.hh"
#include "../lib/c_loops_2d.hh"
#include "../lib/wall_2d.hh"
#include "../lib/cell_nc_2d.hh"
#include "../lib/ctr_boundary_2d.hh"

	
namespace roxlu {

/**
 * Minimalistic wrapper around Voro++2D create by
 * Chris H. Rycroft (LBL / UC Berkeley). See http://math.lbl.gov/voro++/
 * for documentation, source and contact informatio.
 *
 * I'm trying to mimic the usage of Voro++2D w/o creating all kinds of
 * new features. See some of the examples on how to get vertices/edges/faces
 * etc..
 *
 *
 * Examples
 * ------------
 *
 
 	// setup once; fill with points.
	// -----------------------------
	roxlu::Voronoi2D v;
	v.setup(0,1,0,1,10,10,16);
	for(int i = 0; i < 600; ++i) {
		v.put(i,rnd(), rnd());		
	}
	 
	// draw the voronoi cells
	// ---------------------- 
	 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(v.start()) {
		do {
			if(v.computeCell()) {
				glBegin(GL_POLYGON);
				do {
					v.getCellPoint(x,y);
					glVertex2f(x*w,y*h);	
				} while(v.cellHasEdges());
				glEnd();
			}
		} while(v.next());
	}
 
 
 *
 */
class Voronoi2D {
public:

	Voronoi2D();
	~Voronoi2D();

	// All 'sites' must be positioned inside the minx/maxx/miny/maxy boundary.
	void setup(
			 double minX			// min x of bounding box
			,double maxX			// max x of bounding box
			,double minY			// min y of bounding box
			,double maxY			// max y of bounding box
			,int numGridsX			// number of grid cells in x
			,int numGridsY			// number of grid cells in y
			,int numParticlesPerGridCell // how many particles to start per grid
	);
	void clear();
	void put(int i, double x, double y);
	bool start();
	bool next();
	int computeCell();	
	bool cellHasEdges();
	void getCellPoint(float& x, float& y);
	double* getCurrentPoint();
	
	
	voro::container_2d* con;
	voro::c_loop_all_2d* vl;
	voro::voronoicell_2d c;
	int k;
};

inline void Voronoi2D::clear() {
	voro::container_2d* curr = con;
	if(vl != NULL) {
		delete vl;
		vl = NULL;
	}
	con = new voro::container_2d(curr->ax, curr->bx, curr->ay, curr->by, curr->nx, curr->ny, false, false, curr->ps);
	delete curr;
}

inline void Voronoi2D::put(int i, double x, double y) {
	con->put(i, x, y);
}

inline bool Voronoi2D::start() {
	if(con == NULL) {
		return false;
	}
	if(vl != NULL) {
		delete vl;
	}
	vl = new voro::c_loop_all_2d(*con);
	return vl->start();
}

inline void Voronoi2D::getCellPoint(float& x, float& y) {
	double* p = getCurrentPoint();
	x = p[0] + 0.5 * c.pts[2*k];
	y = p[1] + 0.5 * c.pts[2*k+1];
}

inline bool Voronoi2D::next() {
	return vl->inc();
}

inline int Voronoi2D::computeCell() {
	if(!con->compute_cell(c, *vl)) {
		return 0;
	}
	k = 0;
	return c.p;
}

inline bool Voronoi2D::cellHasEdges() {
	k = c.ed[2*k];
	return k != 0;
}

inline double* Voronoi2D::getCurrentPoint() {
	return con->p[vl->ij]+con->ps*vl->q;
}


} // roxlu

#endif
