#include "Axis.h"
namespace roxlu {

Axis::Axis() 
	:vd("axis")
{
}

void Axis::setup(int nSize) {
	int mod = nSize % 2;
	nSize += mod;
	float spacing = (float)nSize*0.05;
	float half_size = nSize * 0.5 * spacing;
	float full_size = half_size * 2;
	int middle = nSize * 0.5;
	Vec3 col_gray(0.5,0.5,0.5);
	Vec3 col_white(1,1,1);
	Vec3 col_x(0.85,0.26,0.29);
	Vec3 col_y(0.56,0.78,0.42);
	Vec3 col_z(0.11,0.64,0.79);
	Vec3* curr_col; 
	for(int i = 0; i <= nSize; ++i) {
		// z-direction
		curr_col = (i == middle) ? &col_z : &col_gray;
		if(i == 0 || i == nSize) {
			curr_col = &col_white;
		}
		vd.addColor(*curr_col);
		vd.addVertex(i * spacing - half_size, 0, half_size);
		vd.addColor(*curr_col);
		vd.addVertex(i * spacing - half_size, 0, -half_size);
		
		
		// x-direction
		curr_col = (i == middle) ? &col_x : &col_gray;
		if(i == 0 || i == nSize) {
			curr_col = &col_white;
		}
		vd.addColor(*curr_col);
		vd.addVertex(half_size, 0, i * spacing - half_size);
		vd.addColor(*curr_col);
		vd.addVertex(-half_size, 0, i * spacing - half_size);
	}
	// draw y 
	vd.addColor(col_y);
	vd.addVertex(0,0,0);
	vd.addColor(col_y);
	vd.addVertex(0,full_size, 0);	
	num_vertices = vd.getNumVertices();
	vbo.setVertexData(vd);
}

void Axis::draw() {
	glLineWidth(1.0);
	glDisable(GL_TEXTURE_2D); eglGetError();
	vbo.drawArrays(GL_LINES, 0, num_vertices); eglGetError();
	//glEnable(GL_TEXTURE_2D); eglGetError();
}

} // roxlu