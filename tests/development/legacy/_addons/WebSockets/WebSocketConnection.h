#ifndef ROXLU_WEBSOCKET_CONNECTIONH
#define ROXLU_WEBSOCKET_CONNECTIONH

extern "C" {
	#include "libwebsockets.h"
}

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace roxlu {

class WebSockets;
class WebSocketProtocol;

class WebSocketConnection {
public:
	WebSocketConnection(WebSockets* websockets, WebSocketProtocol* protocol);
	~WebSocketConnection();
	
	void close();
	void send(const string& msg);
	const string receive(const string& msg);
	
	struct libwebsocket* ws;
	WebSockets* websockets;
	WebSocketProtocol* protocol;
	vector<unsigned char> buffer;
};

} // roxlu
#endif