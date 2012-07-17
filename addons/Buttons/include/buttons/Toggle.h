#ifndef ROXLU_BTOGGLEH
#define ROXLU_BTOGGLEH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

class Toggle : public Element {
public:	
	Toggle(bool& val, const string& name);
	~Toggle();

	void generateStaticText();
	void updateTextPosition();
	void generateVertices(ButtonVertices& shapeVertices);
	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);
	
	void save(std::ofstream& ofs);
	void load(std::ifstream& ifs);
	bool canSave();
	
	Toggle& setColor(const float hue, const float sat, const float bright, const float a = 1.0);
	void hide();
	void show();
	
	bool& value;
	
	float toggle_off_color[4];
	float toggle_on_color[4];
	int label_dx;
};

} // namespace buttons

#endif