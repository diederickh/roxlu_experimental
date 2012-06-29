#include "Tweens.h"
Tweens::Tweens() {
}

Tweens::~Tweens() {
	
	std::map<std::string, Tween*>::iterator it = tweens.begin();
	while(it != tweens.end()) {
		delete it->second;
		++it;
	}

}

void Tweens::update() {
	std::map<std::string, Tween*>::iterator it = tweens.begin();
	while(it != tweens.end()) {
		it->second->update();
		++it;
	}
}