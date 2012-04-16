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
	void generateVertices(ButtonVertices& shapeVertices);
	void setup();
	void draw();
	void update();
	void onMouseEnter(int x, int y);
	void onMouseLeave(int x, int y);	

	float& value;
	float bg_color[4];

};

} // namespace buttons

#endif