#ifndef ROXLU_BRADIOH
#define ROXLU_BRADIOH

#include <string>
#include <vector>

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Toggle.h>

using std::string;
using std::vector;

namespace buttons {

template<class T>

class Radio : public Element {
public:	
	Radio(int id, const vector<string>& options, int& selected, const string& name, T* cb); 
	~Radio();

	void addOption(Toggle* toggle);
	vector<Toggle*>& getOptions();

	void generateStaticText(Text& txt);
	void updateTextPosition(Text& staticText, Text& dynamicText);
	void generateVertices(ButtonVertices& shapeVertices);
	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);
	void onSaved();
	void onLoaded();

	bool updateSelected();

	//float bg_top_color[4];
	//float bg_bottom_color[4];
	int label_dx;
	const vector<string>& options;
	T* cb;

	int& selected;
	bool* values; // array of bool for each toggle
	vector<Toggle*> toggles;
	int id;
};

template<class T>
Radio<T>::Radio(int id, const vector<string>& options, int& selected, const string& name, T* cb) 
	:Element(BTYPE_RADIO, name, BVALUE_NONE)
	,options(options)
	,cb(cb)
	,id(id)
	,selected(selected)
	,label_dx(0)
	,values(NULL)
{
	h = 20;
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	values = new bool[options.size()];
	
}

template<class T>
Radio<T>::~Radio() {
}

template<class T>
void Radio<T>::addOption(Toggle* toggle) {
	toggles.push_back(toggle);
}

template<class T>
vector<Toggle*>& Radio<T>::getOptions() {
	return toggles;
}

template<class T>
void Radio<T>::generateStaticText(Text& txt) {
	label_dx = txt.add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
}

template<class T>
void Radio<T>::updateTextPosition(Text& staticText, Text& dynamicText) {
	staticText.setTextPosition(label_dx, x+4, y+2);
}

template<class T>
void Radio<T>::generateVertices(ButtonVertices& vd) {
	num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
}

template<class T>
void Radio<T>::onMouseDown(int mx, int my) {
	bool was_inside = updateSelected();
	if(was_inside) {
		(*cb)(id);
	}	
}

template<class T>
bool Radio<T>::updateSelected() {
	bool was_inside = false;
	
	for(int i = 0; i < toggles.size(); ++i) {
		Toggle& toggle = *toggles[i];
		if(toggle.is_mouse_inside && was_inside == false) {
			was_inside = true;
			selected = i;
			printf("selected: %d\n", selected);
		}
	}
	
	if(was_inside) {
		for(int i = 0; i < toggles.size(); ++i) {
			Toggle& toggle = *toggles[i];
			if(i != selected) {
				toggle.value = false;
				toggle.needsRedraw();
			}
		}
	}
	return was_inside;
}


template<class T>
void Radio<T>::onMouseUp(int mx, int my) {
}

template<class T>
void Radio<T>::onMouseEnter(int mx, int my) {
}

template<class T>
void Radio<T>::onMouseLeave(int mx, int my) {
}

template<class T>
void Radio<T>::onMouseClick(int mx, int my) {
}

template<class T>
void Radio<T>::onSaved() {
}

template<class T>
void Radio<T>::onLoaded() {
	for(int i = 0; i < toggles.size(); ++i) {
		Toggle& toggle = *toggles[i];
		if(toggle.value) {
			selected = i;
			break;
		}
	}
}




} // namespace buttons

#endif