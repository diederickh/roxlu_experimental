//#include "SimpleAnim.h"

#include <roxlu/experimental/SimpleAnim.h>
#include <math.h>
namespace roxlu {
	
SimpleAnim::SimpleAnim()
	:mode(ANIM_NONE)
	,frame(0)
	,total_frames(0)
{
}

bool SimpleAnim::load(const string& filepath) {
	std::ifstream ifs;
	ifs.open(filepath.c_str());
	if(!ifs.is_open()) {
		return false;
	}
	
	string line;
	string object;
	bool found_object = false;
	AnimData ad;
	
	while(getline(ifs, line)) {
		int space = line.find(" ");
		if(space == string::npos) {
			continue;
		}
		
		string cmd = line.substr(0, space);
		
		// command
		if(cmd == "o") {
			found_object = true;
			object = line.substr(space+1, line.size()-1);
			vector<AnimData> ad;
			animations.insert(std::pair<string, vector<AnimData> >(object, ad));
		}
		// position	
		else if(found_object && cmd == "p") {
			std::stringstream ss;
			ss << line.substr(space+1, line.size()-1);
			ss >> ad.pos[0] >> ad.pos[1] >> ad.pos[2];
			//ad.print();
			animations[object].push_back(ad);
			total_frames = animations[object].size();
		}
	}
	
	setFPS(12);
	
	return true;
}

void SimpleAnim::setFPS(const unsigned int& fps) {
	millis_per_frame = (1.0f/fps) * 1000;
}

void SimpleAnim::play() {
	mode = ANIM_PLAY;
	next_frame = now() + millis_per_frame;
}

void SimpleAnim::update() {
	switch(mode) {
		case ANIM_PLAY: {
			rx_int64 millis_now = now();
			rx_int64 millis_dif = next_frame - millis_now;
			millis_dif *= millis_dif;
			millis_dif = sqrtf(float(millis_dif));
			perc = float(millis_dif)/millis_per_frame;
			frame = int(perc) % total_frames; // restart every time
	
			perc = perc - int(perc);
			break;
		}
		default:break;
	}
	
}


void SimpleAnim::getPosition(const string& group, float* dest) {
	map<string, vector<AnimData> >::iterator it = animations.find(group);
	if(it == animations.end()) {
		printf("Cannot find: %s\n", group.c_str());
		return;	
	}
	
	if(frame < total_frames-1) {
		//it->second[frame].print();
		float* a = it->second[frame].pos;
		float* b = it->second[frame+1].pos;
		float dx = b[0] - a[0];
		float dy = b[1] - a[1];
		float dz = b[2] - a[2];
		dest[0] = a[0] + perc * dx;
		dest[1] = a[1] + perc * dy;
		dest[2] = a[2] + perc * dz;
	}
	else {
		dest = it->second[frame].pos;
	}
}

void SimpleAnim::addFrame(const string& group, const float* pos) {
	AnimData ad;
	ad.setPos(pos);
	animations[group].push_back(ad);
}

bool SimpleAnim::save(const string& filepath) {
	std::ofstream ofs;
	ofs.open(filepath.c_str());
	if(!ofs.is_open()) {
		printf("Cannot save animation data to %s\n", filepath.c_str());
		return false;
	}
	
	map<string, vector<AnimData> >::iterator it = animations.begin();
	string group = "";
	while(it != animations.end()) {
		group = it->first;
		ofs << "o " << group << "\n";
		vector<AnimData>& data = it->second;
		vector<AnimData>::iterator dit = data.begin();
		while(dit != data.end()) {
			AnimData& ad = *dit;
			ofs << "p " << ad.pos[0] << " " << ad.pos[1] << " " << ad.pos[2] << "\n";
			++dit;
		}
		++it;
	}
	ofs.close();
	return true;
}

bool SimpleAnim::hasGroup(const string& group) {
	map<string, vector<AnimData> >::iterator it = animations.find(group);
	return (it != animations.end());
}

} // roxlu	