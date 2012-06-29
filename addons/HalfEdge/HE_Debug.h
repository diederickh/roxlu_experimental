#ifndef HE_DEBUGH
#define HE_DEBUGH

namespace roxlu {
class HE_Mesh;

class HE_Debug {
public:
	HE_Debug();
	static void drawPaired(HE_Mesh& m);
	static void drawEdges(HE_Mesh& m);
	static void drawUnpaired(HE_Mesh& m);
	static void drawFaceNormals(HE_Mesh& m, float len = 1.0);
	static void drawFacesWithRandomColors(HE_Mesh& m);
	static void drawOuterEdges(HE_Mesh& m); 
	static void drawInnerEdges(HE_Mesh& m);
	static void drawOuterVertices(HE_Mesh& m);
	static void drawInnerVertices(HE_Mesh& m);
	
	static void printStats(HE_Mesh& m);
	static void printVertexLabels(HE_Mesh& m);
};

}; // roxlu
#endif