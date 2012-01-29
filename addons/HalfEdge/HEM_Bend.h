#ifndef HEM_BENDH
#define HEM_BENDH

#include "HEM_Modifier.h"

namespace roxlu {

class HE_Mesh;

class HEM_Bend : public HEM_Modifier {
public:
	HEM_Bend();
	bool modify(HE_Mesh& m);
};

}; // roxlu
#endif