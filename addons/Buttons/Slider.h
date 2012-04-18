#ifndef ROXLU_BSLIDERH
#define ROXLU_BSLIDERH

#include "ofMain.h"
#include "Types.h"
#include "Element.h"

namespace buttons {

class Slider : public Element {
public:	
	Slider(float& val);
	~Slider();

	void generateStaticText(Text& txt);
	void updateTextPosition(Text& staticText, Text& dynamicText);
	void generateDynamicText(Text& dynText);
	void updateDynamicText(Text& dynText);
	void generateVertices(ButtonVertices& shapeVertices);


	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseDragged(int mx, int my, int dx, int dy);

	void setValue(const float& v);

	float& value;
	float minv; // min value
	float maxv; // max value 
	float p; // current percentage
	
	float bg_top_color[4];
	float bg_bottom_color[4];
	float bar_filled_color[4];
	float bar_empty_color[4];
	int txtval_dx;
	int label_dx;
};

} // namespace buttons

#endif