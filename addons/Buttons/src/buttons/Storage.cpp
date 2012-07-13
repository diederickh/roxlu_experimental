#include <buttons/Storage.h>
#include <buttons/Buttons.h>

namespace buttons {

bool Storage::save(const string& file, Buttons* buttons) {
	std::ofstream ofs(file.c_str(), std::ios::out | std::ios::binary);
	if(!ofs.is_open()) {
		printf("Cannot save: %s\n", file.c_str());
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
		if(!e.canSave()) {
			++it;
			continue;
		}
		
		// store type
		ofs.write((char*)&e.type, sizeof(int));
		
		// store name.
		size_t name_size = e.name.length()+1;
		ofs.write((char*)&name_size, sizeof(size_t));
		ofs.write((char*)e.name.c_str(), name_size);
		
		e.save(ofs);
		
		++it;
	}
	ofs.close();
	return true;
}
// @todo make this work when one adds/removes elements to gui! 
// crashes on windows when you save with 10 elements en load 
// with 8 for example.
bool Storage::load(const string& file, Buttons* buttons) {
	
	std::ifstream ifs(file.c_str(), std::ios::out | std::ios::binary);
	if(!ifs.is_open()) {
		printf("Cannot load: %s\n", file.c_str());
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

		size_t data_size;
		ifs.read((char*)&data_size, sizeof(size_t));
		
		if(el == NULL) {
			printf("%s not found, forgetting about value...\n", name_buf);
			ifs.seekg(data_size, ios_base::cur);
		}
		else {
			el->load(ifs);
		}
	}
	ifs.close();
	return true;
}

} // namespace buttons


