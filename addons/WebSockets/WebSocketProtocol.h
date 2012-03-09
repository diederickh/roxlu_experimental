#ifndef ROXLU_WEBSOCKETPROTOCOLH
#define ROXLU_WEBSOCKETPROTOCOLH

#include "libwebsockets.h"
#include <string>
#include "WebSocketConnection.h"

using std::string;

namespace roxlu {

class WebSockets;

class WebSocketProtocol {

public:

	WebSocketProtocol();
	~WebSocketProtocol();
	void setName(const string& name);
	string getName();
	void setProtocolIndex(const unsigned int dx);
	unsigned int getProtocolIndex();
	void broadcast(const string& msg);

	virtual void onOpen(string& msg, WebSocketConnection& conn) {};
	virtual void onClose(string& msg, WebSocketConnection& conn) {};
	virtual void onIdle(string& msg, WebSocketConnection& conn) {};
	virtual void onMessage(string& msg, WebSocketConnection& conn) {};
	virtual void onBroadcast(string& msg, WebSocketConnection& conn) {};
	virtual bool allowClient(const string& name, const string& ip) const {	return true; }
	
	WebSockets* websockets;

private:
	
	unsigned int protocol_dx;
	string name;
	
};

inline void WebSocketProtocol::setName(const string& n) {
	name = n;
}

inline string WebSocketProtocol::getName() {
	return name;
}

inline void WebSocketProtocol::setProtocolIndex(const unsigned int dx) {
	protocol_dx = dx;
}

inline unsigned int WebSocketProtocol::getProtocolIndex() {
	return protocol_dx;
}



} // roxlu
#endif