#include "OSCSender.h"

namespace roxlu {

OSCSender::OSCSender(const string& hostname, int port) {
	socket = new UdpTransmitSocket(IpEndpointName( hostname.c_str(), port ) );
}

OSCSender::~OSCSender() {
	if(socket) {
		shutdown();
	}
}

void OSCSender::shutdown() {
	if(!socket) {
		return;
	}
	delete socket;
	socket = NULL;
}

void OSCSender::sendMessage(OSCMessage& msg) {
	static const int OUTPUT_BUFFER_SIZE = 16384;
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
	
	p << osc::BeginBundleImmediate;
		appendMessage(msg, p);
	p << osc::EndBundle;
	
	socket->Send(p.Data(), p.Size());
	
}

void OSCSender::appendMessage(OSCMessage& msg, osc::OutboundPacketStream& p) {
	p << osc::BeginMessage(msg.getAddress().c_str());

	OSCMessage::iterator it = msg.begin();
	int i = 0;
	while(it != msg.end()) {
		OSCArg* oscm = (*it);
		OSCArgType type = oscm->getType();
		switch(type) {
			case OSCARG_TYPE_INT32: { 
				p << msg.getInt32(i);
				break;
			}
			case OSCARG_TYPE_FLOAT: {
				p << msg.getFloat(i);
				break;
			}
			case OSCARG_TYPE_STRING: {
				p << msg.getString(i).c_str();
				break;
			}
			default:break;
		}
		++i;
		++it;
	}	
	
	p << osc::EndMessage;

}

} // roxlu