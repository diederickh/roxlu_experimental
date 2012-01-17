#include "TwitterCurlValues.h"
#include "TwitterCurlValueType.h"
#include "TwitterCurlValueTypeFile.h"

namespace roxlu {

TwitterCurlValues::TwitterCurlValues() {
}

TwitterCurlValues::~TwitterCurlValues() {
	vector<TwitterCurlValueType*>::iterator it = values.begin();
	while(it != values.end()) {
		delete *it;
		it = values.erase(it);
	}
}



void TwitterCurlValues::addFile(const string& name, const string& filePath) {
	TwitterCurlValueTypeFile* v = new TwitterCurlValueTypeFile(name, filePath);
	values.push_back(v);
}


}; // roxlu