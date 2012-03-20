#include "OSCGui.h"
namespace roxlu {

// special line to test submodules

OSCGui::OSCGui(int receiverPort, int senderPort)
	:osc_receiver(receiverPort)
	,osc_sender("localhost", senderPort)
	,value_changed(false)
{
}

OSCGui::~OSCGui() {
	map<string, OSCGType*>::iterator it = elements.begin();
	while(it != elements.end()) {
		delete it->second;
		++it;
	}
}

void OSCGui::addFloat(const string& name, float& f) {
	OSCGFloat* el = new OSCGFloat(name, f);
	elements[name] = el;
}

void OSCGui::addBool(const string& name, bool& b) {
	OSCGBool* el = new OSCGBool(name, b);
	elements[name] = el;
}

void OSCGui::addColor(const string& name, float* rgba) {
	addColor(name, *rgba, *(rgba+1), *(rgba+2), *(rgba+3));
}

void OSCGui::addColor(const string& name, float& r, float& g, float& b, float& a) {
	OSCGColor* el = new OSCGColor(name, r, g, b, a);
	elements[name] = el;
}

void OSCGui::addInt(const string& name, int& v) {
	OSCGInt* el = new OSCGInt(name, v);
	elements[name] = el;
}

void OSCGui::addString(const string& name, string& value) {
	OSCGString* el = new OSCGString(name, value);
	elements[name] = el;
}

void OSCGui::addCallback(const string& name, int value, GuiCallback* type) {
	OSCGCallback* el = new OSCGCallback(name, value, type);
	elements[name] = el;
}


void OSCGui::update() {
	value_changed = false;
	OSCMessage m;
	if(osc_receiver.hasMessages()) {
		while(osc_receiver.getNextMessage(&m)) {
			value_changed = true;
			int32_t oscu_type = m.getInt32(0);
			string oscu_name = m.getString(1);
	
			switch(oscu_type) {
				case OSCU_SLIDER: {
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					((OSCGFloat*)it->second)->setValue(m.getFloat(2));
					break;
				}
				case OSCU_BOOL: {
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					((OSCGBool*)it->second)->setValue(m.getInt32(2));
					break;
				}
				case OSCU_COLOR: {
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					((OSCGColor*)it->second)->setValue(
						 m.getFloat(2)
						,m.getFloat(3)
						,m.getFloat(4)
						,m.getFloat(5)
					);
					break;
				}
				// Radio list
				case OSCU_MATRIX: {
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					((OSCGInt*)it->second)->setValue(m.getInt32(2));
					break;
				}
				case OSCU_STRING: {	
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					((OSCGString*)it->second)->setValue(m.getString(2));
					break;
				}
				case OSCU_CALLBACK: {	
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					((OSCGCallback*)it->second)->call();
					break;
				}
				
				// Control messages
				case OSCU_COMMAND: {
					uint32_t command = m.getInt32(1);
					if(command == OSCU_COMMAND_LOAD_SETTINGS) {
						sendValues();
					}
					break;
				}
				default: {
					printf("warning: unhandled osculator type.\n");
					break;
				}
			};

		}
	}
}


bool OSCGui::save(const string& filepath) {
	std::ofstream ofs(filepath.c_str());
	if(!ofs.is_open()) {
		printf("Cannot open: '%s'\n", filepath.c_str());
		return false;
	}
	
	std::stringstream ss;
	string val;
	map<string, OSCGType*>::iterator it = elements.begin();
	while(it != elements.end()) {
		OSCGType* type = it->second;
		string name = it->first;
		if(type->getStringValue(val)) {
			ss << name.c_str() << "=" << val << std::endl;
		}
		++it;
	}
	ofs.write(ss.str().c_str(), ss.str().size()+1);
	ofs.close();
	return true;
}

bool OSCGui::load(const string& filepath) {
	std::ifstream ifs(filepath.c_str());
	if(!ifs.is_open()) {
		printf("Cannot open: '%s'\n", filepath.c_str());
		return false;
	}
	string name;
	string value;
	string line;
	while(std::getline(ifs, line)) {
		line = trim(line);
		if(!line.size()) {
			continue;
		}
		std::stringstream ss(line);
		getline(ss, name, '=');
		getline(ss, value,'=');
		if(!name.size() || !value.size()) {
			continue;
		}
		map<string, OSCGType*>::iterator it = elements.find(name);
		if(it == elements.end()) {
			continue;
		}
		
		it->second->setStringValue(value);
		name.clear();
		value.clear();
	}
	sendValues();
}

void OSCGui::sendValues() {
	OSCMessage msg;
	msg.addInt32(OSCU_COMMAND);
	msg.addInt32(OSCU_COMMAND_LOAD_SETTINGS);
	string val;
	map<string, OSCGType*>::iterator it = elements.begin();
	while(it != elements.end()) {
		if(it->second->getStringValue(val)) {
			msg.addString(it->first);
			msg.addString(val);
		}
		++it;
	}
	osc_sender.sendMessage(msg);
}


} // roxlu