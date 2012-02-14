#ifndef ROXLU_OPENNI_PLAYER_H
#define ROXLU_OPENNI_PLAYER_H

#include "Generator.h"
#include <OpenNI/XnCppWrapper.h>
#include <OpenNI/XnTypes.h>
#include <string> 


namespace ro = roxlu::openni;

namespace roxlu {
namespace openni {

//class Player : public ro::Generator{

class Player {
public:

	Player();
	~Player();
///	virtual void update();
	xn::Player& getPlayer();
	
private:
	
	xn::Player player;
	
};

inline xn::Player& Player::getPlayer() {
	return player;
}

}} // roxlu openni

#endif