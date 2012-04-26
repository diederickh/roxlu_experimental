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
		
		// store type
		ofs.write((char*)&e.type, sizeof(int));
		
		// store name.
		size_t name_size = e.name.length()+1;
		ofs.write((char*)&name_size, sizeof(size_t));
		ofs.write((char*)e.name.c_str(), name_size);
		
		switch(e.type) {
			case BTYPE_SLIDER: {
				int value_type = (*it)->getValueType();
				if(value_type == BVALUE_INT) {
					Slideri* slider = static_cast<Slideri*>((*it));
					ofs.write((char*)&value_type, sizeof(int));
					ofs.write((char*)&slider->value, sizeof(float));
				}
				else if(value_type == BVALUE_FLOAT) {
					Sliderf* slider = static_cast<Sliderf*>((*it));
					ofs.write((char*)&value_type, sizeof(int));
					ofs.write((char*)&slider->value, sizeof(float));
				}
				break;
			}
			case BTYPE_TOGGLE: {
				Toggle* toggle = static_cast<Toggle*>((*it));
				ofs.write((char*)&toggle->value, sizeof(bool));
				break;
			}
			default: {
				printf("Cannot store unhandled gui type: %d\n", e.type);
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
	char name_buf[1024];
	for(int i = 0; i < num_els; ++i) {
		int type = 0;
		ifs.read((char*)&type, sizeof(int));
		
		// retrieve name.
		size_t name_size;
		ifs.read((char*)&name_size, sizeof(size_t));
		ifs.read((char*)name_buf, name_size);
		
		Element* el = buttons->getElement(name_buf);
		if(el == NULL) {
			continue;
		}

		switch(type) {
			case BTYPE_SLIDER: {
				int value_type = BVALUE_NONE;
				ifs.read((char*)&value_type, sizeof(int));
				if(value_type == BVALUE_FLOAT) {
					Sliderf* slider = static_cast<Sliderf*>(el);
					ifs.read((char*)&slider->value, sizeof(float));
					slider->setValue(slider->value);
					slider->needsRedraw();
					slider->needsTextUpdate();
				}
				else if(value_type == BVALUE_INT) {
					Slideri* slider = static_cast<Slideri*>(el);
					ifs.read((char*)&slider->value, sizeof(int));
					slider->setValue(slider->value);
					slider->needsRedraw();
					slider->needsTextUpdate();
				}
				break;
			}
			case BTYPE_TOGGLE: {
				Toggle* toggle = static_cast<Toggle*>(el);
				ifs.read((char*)&toggle->value, sizeof(bool));
				toggle->needsRedraw();
				break;
			}
			default: {
				printf("Cannot load gui type: %d\n", type);
				break;
			}	
				
		}	
	}
	ifs.close();
	return true;
}

} // namespace buttons


