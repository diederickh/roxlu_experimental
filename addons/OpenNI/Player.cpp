#include "Player.h"

namespace roxlu {
namespace openni {

Player::Player() {
}

Player::~Player() {
	player.Release();
}

//void Player::update() {
//}


}} // roxlu::openni