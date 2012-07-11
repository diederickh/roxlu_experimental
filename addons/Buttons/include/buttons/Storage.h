#ifndef ROXLU_BUTTONS_STORAGEH
#define ROXLU_BUTTONS_STORAGEH

#include <string>
#include <fstream>

using std::string;

namespace buttons {

class Buttons;

class Storage {

public: 
	Storage() {}
	~Storage() { printf("Storage::~Storage()\n"); } 
	bool save(const string& file, Buttons* b);
	bool load(const string& file, Buttons* b);
};

} // namespace buttons
#endif