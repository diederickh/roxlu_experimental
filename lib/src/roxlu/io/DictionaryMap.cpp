//#include "DictionaryMap.h"
//#include "Dictionary.h"
//#include "ofMain.h"

#include <roxlu/io/Dictionary.h>
#include <roxlu/io/DictionaryMap.h>

namespace roxlu {

DictionaryMap::DictionaryMap() {
	is_array = false;
}

DictionaryMap::~DictionaryMap() {
}

DictionaryMap::DictionaryMap(DictionaryMap& other) {
	type_name = other.type_name;
	children = other.children;
	is_array = other.is_array;
}

} // namespace roxlu