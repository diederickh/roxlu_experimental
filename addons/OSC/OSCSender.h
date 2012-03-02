#ifndef ROXLU_OSC_SENDERH
#define ROXLU_OSC_SENDERH

#include <string>

#include "OSCMessage.h"
#include "OscTypes.h"
#include "UdpSocket.h"
#include "OscOutboundPacketStream.h"

using std::string;

namespace roxlu {

class OSCSender {
public:
	OSCSender(const string& hostname, int port);
	~OSCSender();
	
	void sendMessage(OSCMessage& msg);
	
private:
	void shutdown();
	void appendMessage(OSCMessage& msg, osc::OutboundPacketStream& p);
	UdpTransmitSocket* socket;
};

} // roxlu
#endif