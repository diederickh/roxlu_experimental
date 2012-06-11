#ifndef ROXLU_RELEMENTH
#define ROXLU_RELEMENTH

#include "Text.h"
#include "Types.h"
#include <string>

using namespace roxlu;

using std::string;

namespace buttons {

class Element {
public:	
	Element(int type, const string& name, int valueType);
	~Element();

	virtual void setup(){}
	virtual void update(){}
	virtual void generateStaticText(Text& staticText) {}
	virtual void updateTextPosition(Text& staticText, Text& dynText) {}
	virtual void generateDynamicText(Text& dynText) {};
	virtual void updateDynamicText(Text& dynText) {};
	virtual void generateVertices(ButtonVertices& shapeVertices) = 0;
	
	
	virtual void onMouseMoved(int mx, int my) { } // global  move
	virtual void onMouseDragged(int mx, int my, int dx, int dy) { } // global drag (check: drag_inside)
	virtual void onMouseDown(int mx, int my) { } // global down
	virtual void onMouseUp(int mx, int my) { } // global up
	virtual void onMouseClick(int mx, int my) { } // when click and released inside
	virtual void onMouseEnter(int mx, int my) { }
	virtual void onMouseLeave(int mx, int my) { }
	
	virtual void onSaved(){}  // gets called once all data has been saved
	virtual void onLoaded(){}  // gets called once all data has been loaded 
	
	void setValueType(int valueType);
	int getValueType();
	void needsRedraw();
	void needsTextUpdate(); // when you want to change the dynamic text
	
	int x;
	int y; 
	int w;
	int h;
	int type;
	int value_type; // what kind of data do you store.

	string label;
	string name;
	int state;
	bool is_mouse_inside;
	bool is_mouse_down_inside;
	bool needs_redraw;	
	bool needs_text_update;
	bool drag_inside; // did the drag started from inside the element.
	
	int num_vertices;
};

inline void Element::needsRedraw() {
	needs_redraw = true;
}

inline void Element::needsTextUpdate() {
	needs_text_update = true;
}

inline void Element::setValueType(int valueType) {
	value_type = valueType;
}

inline int Element::getValueType() {
	return value_type;
}

} // namespace buttons

#endif