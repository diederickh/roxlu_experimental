#ifndef HEC_CREATORH
#define HEC_CREATORH

namespace roxlu {

class HE_Mesh;

class HEC_Creator {
public:
	HEC_Creator(){}
	virtual bool create(HE_Mesh& mesh) = 0;
};

};
#endif