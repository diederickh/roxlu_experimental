#include "Collection.h"

namespace roxlu {
namespace twitter {
namespace parameter {

Collection::Collection() {
}

Collection::~Collection() {
	list<Parameter*>::iterator it = params.begin();
	while(it != params.end()) {
		delete *it;
		it = params.erase(it);
	}	
}

void Collection::print() const {
	printf("print\n");
	list<Parameter*>::const_iterator it = params.begin();
	while(it != params.end()) {
		(*it)->print();
		printf("\n");
		++it;
	}	
}

}}} // roxlu::twitter::parameter