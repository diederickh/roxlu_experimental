#include "Storage.h"
#include "Buttons.h"
namespace buttons {

bool Storage::save(const string& file, Buttons* buttons) {
	std::ofstream ofs(file.c_str(), std::ios::out | std::ios::binary);
	if(!ofs.is_open()) {
		return false;
	}
	
	// Panel specific settings.
	Buttons& b = *buttons;
	ofs.write((char *)&b.x, sizeof(int));
	ofs.write((char *)&b.y, sizeof(int));
	ofs.write((char *)&b.w, sizeof(int));
	
	// Write number of elements.
	size_t num_els = b.elements.size();
	ofs.write((char*)&num_els, sizeof(size_t));
	
	// Elements
	vector<Element*>::iterator it = b.elements.begin();
	while(it != b.elements.end()) {
		Element& e = *(*it);
		ofs.write((char*)&e.type, sizeof(int));
		switch(e.type) {
			case BTYPE_SLIDER: {
				Slider* slider = static_cast<Slider*>((*it));
				ofs.write((char*)&slider->value, sizeof(float));
				break;
			}
			case BTYPE_TOGGLE: {
				Toggle* toggle = static_cast<Toggle*>((*it));
				ofs.write((char*)&toggle->value, sizeof(bool));
				break;
			}
			default: {
				printf("Cannot store unhandled type: %d\n", e.type);
				break;
			}
		}
		++it;
	}
	ofs.close();
	return true;
}

bool Storage::load(const string& file, Buttons* buttons) {
	std::ifstream ifs(file.c_str(), std::ios::out | std::ios::binary);
	if(!ifs.is_open()) {
		return false;
	}
	
	// Panel specific settings.
	Buttons& b = *buttons;
	int x, y, w;
	ifs.read((char*)&x, sizeof(int));
	ifs.read((char*)&y, sizeof(int));
	ifs.read((char*)&w, sizeof(int));
	b.setPosition(x,y);
	
	// Number of elements.
	size_t num_els = 0;
	ifs.read((char*)&num_els, sizeof(size_t));
	
	// Load elements.
	for(int i = 0; i < num_els; ++i) {
		int type = 0;
		ifs.read((char*)&type, sizeof(int));
		switch(type) {
			case BTYPE_SLIDER: {
				Slider* slider = static_cast<Slider*>(b.elements[i]);
				ifs.read((char*)&slider->value, sizeof(float));
				slider->setValue(slider->value);
				slider->needsRedraw();
				slider->needsTextUpdate();
				break;
			}
			case BTYPE_TOGGLE: {
				Toggle* toggle = static_cast<Toggle*>(b.elements[i]);
				ifs.read((char*)&toggle->value, sizeof(bool));
				toggle->needsRedraw();
				break;
			}
			default: {
				printf("Cannot load type: %d\n", type);
				break;
			}	
				
		}	
	}
	ifs.close();
	return true;
}

} // namespace buttons


