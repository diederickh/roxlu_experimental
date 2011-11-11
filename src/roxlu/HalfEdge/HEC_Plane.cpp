#include "HEC_Plane.h"
#include "HE_Mesh.h"
#include "HE_Headers.h"
#include "Vec3.h"
//#include "ofMain.h"

namespace roxlu {

HEC_Plane::HEC_Plane() 	
	:HEC_Creator()
	,num_rows(1)
	,num_cols(1)
	,width(1)
	,height(1)
{
}

bool HEC_Plane::create(HE_Mesh& mesh) {
	HEC_FromTriangles from_tri;
	
	// create vertices
	float hw = -0.5 * width;
	float hh = -0.5 * height;
	float dw = width/num_cols;
	float dh = height/num_rows;

	
	for(int j = 0; j <= num_rows; ++j) {
		for(int i = 0; i <= num_cols; ++i) {
			float y = hh + (j * dh);
			float x = hw + (i * dw);
			from_tri.addVertex(x,y,0);
		}
	}
	
	// create triangles.
	vector<int> indices;
	for(int j = 0; j < num_rows; ++j) {
		for(int i = 0; i < num_cols; ++i) {
			// first triangle.
			int a = HDX(i,j, num_cols+1); // num_cols+1 => complete width of array
			int b = HDX(i+1, j, num_cols+1);
			int c = HDX(i, (j+1), num_cols+1);
			from_tri.addTriangle(a,b,c);

			// second triangle.
			a = HDX(i+1, j, num_cols+1);
			b = HDX(i+1, j+1, num_cols+1);
			c = HDX(i, j+1, num_cols+1);
			from_tri.addTriangle(a,b,c);
		}
	}
	return from_tri.create(mesh);
}

}; // roxlu