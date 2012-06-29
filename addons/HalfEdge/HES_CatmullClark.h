#ifndef HES_CATMULLCLARKH
#define HES_CATMULLCLARKH

#include "HES_Subdividor.h"
namespace roxlu {

class HE_Mesh;
class HES_CatmullClark : public HES_Subdividor {
public:
	HES_CatmullClark();
	bool apply(HE_Mesh& m);
	inline HES_CatmullClark& setKeepBoundary(bool keep);
private:
	bool keep_boundary;
};

inline HES_CatmullClark& HES_CatmullClark::setKeepBoundary(bool keep) {
	keep_boundary = keep;
	return *this;
}

}; // roxlu
#endif