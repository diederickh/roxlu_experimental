#ifndef ROXLU_PLANEH
#define ROXLU_PLANEH
namespace roxlu {

class VertexData;
class Plane {
public:	
	Plane();
	~Plane();
	void create(float width, float height, VertexData& vd);
};

} // roxlu
#endif