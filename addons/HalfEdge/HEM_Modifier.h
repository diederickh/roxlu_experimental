#ifndef HEM_MODIFIERH
#define HEM_MODIFIERH

namespace roxlu {

class HE_Mesh;
class HEM_Modifier {
public:
	HEM_Modifier() {}
	virtual bool modify(HE_Mesh& m) = 0;
};

}; // roxlu
#endif