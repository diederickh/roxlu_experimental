#ifndef ROXLU_RSLIDERH
#define ROXLU_RSLIDERH

#include "ofMain.h"
#include "RTypes.h"
#include "RElement.h"


class RSlider : public RElement {
public:	
	RSlider(float& val);
	~RSlider();
// Text& staticText
	void generateStaticText(Text& txt);
	void generateVertices(RuiVertices& shapeVertices);
	void setup();
	void draw();
	void update();
	void onMouseEnter(int x, int y);
	void onMouseLeave(int x, int y);	

/*
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void getVertices(VertexData& vd) = 0;
*/
	float& value;
	float bg_color[4];

};

#endif