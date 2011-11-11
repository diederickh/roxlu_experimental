#ifndef HES_SUBDIVIDORH
#define HES_SUBDIVIDORH

namespace roxlu {

class HE_Mesh;
class HES_Subdividor {
public:
	HES_Subdividor();
	virtual bool apply(HE_Mesh& m) = 0;
};

}; // roxlus
#endif