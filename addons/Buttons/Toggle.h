#ifndef ROXLU_BTOGGLEH
#define ROXLU_BTOGGLEH

#include "Types.h"
#include "Element.h"

namespace buttons {

class Toggle : public Element {
public:	
	Toggle(bool& val, const string& name);
	~Toggle();

	void generateStaticText(Text& txt);
	void updateTextPosition(Text& staticText, Text& dynamicText);
	void generateVertices(ButtonVertices& shapeVertices);
	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);

	bool& value;
	
	float bg_top_color[4];
	float bg_bottom_color[4];
	float toggle_off_color[4];
	float toggle_on_color[4];
	int label_dx;
};

} // namespace buttons

#endif