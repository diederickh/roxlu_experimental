#include "OSCGui.h"
namespace roxlu {

OSCGui::OSCGui(int port)
	:osc_receiver(port)
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

void OSCGui::update() {
	OSCMessage m;
	if(osc_receiver.hasMessages()) {
		while(osc_receiver.getNextMessage(&m)) {
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
					printf("COLOR\n");
					map<string, OSCGType*>::iterator it = elements.find(oscu_name);
					if(it == elements.end()) {
						return;
					}
					printf("%f\n", m.getFloat(2));
					((OSCGColor*)it->second)->setValue(
						 m.getFloat(2)
						,m.getFloat(3)
						,m.getFloat(4)
						,m.getFloat(5)
					);
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

} // roxlu