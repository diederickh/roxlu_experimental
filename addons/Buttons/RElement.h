#ifndef ROXLU_RELEMENTH
#define ROXLU_RELEMENTH

#include "Text.h"
#include "RTypes.h"
#include <string>

using namespace roxlu;

using std::string;

enum RStates {
	 RSTATE_NONE
	,RSTATE_ENTER
	,RSTATE_LEAVE
};

class RElement {
public:	
	RElement();
	~RElement();
	
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void generateStaticText(Text& staticText) = 0;
	virtual void generateVertices(RuiVertices& shapeVertices) = 0;
	
	
	virtual void onMouseMoved(int x, int y) { }
	virtual void onMouseDragged(int dx, int dy) { }
	virtual void onMousePressed(int x, int y) { }
	virtual void onMouseClick(int x, int y) { } // when click and released inside
	virtual void onMouseEnter(int x, int y) { }
	virtual void onMouseLeave(int x, int y) { }
	
	void flagChanged();
	
	int x;
	int y; 
	int w;
	int h;

	string label;
	int state;
	bool is_mouse_inside;
	bool is_mouse_down_inside;
	bool is_changed;	
	
	int start_dx;
	int end_dx;
	int num_vertices;
};

inline void RElement::flagChanged() {
	is_changed = true;
}

#endif