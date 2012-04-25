#ifndef APOLLO_TWEENSH
#define APOLLO_TWEENSH

#include <map>
#include <string>
#include "Tween.h"


class Tweens {
public:
	Tweens();
	~Tweens();
	void update();
	Tween& operator[](const std::string& name);
	std::map<std::string, Tween*> tweens;
};

inline Tween& Tweens::operator[](const std::string& name) {
	std::map<std::string, Tween*>::iterator it = tweens.find(name);
	if(it != tweens.end()) {
		return *(it->second);
	}
	Tween* t = new Tween();
	tweens[name] = t;
	return *t;
}

#endif