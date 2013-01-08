#include "WebSocketProtocol.h"
#include "WebSockets.h"

namespace roxlu {

WebSocketProtocol::WebSocketProtocol()
	:websockets(NULL) 
{
}

WebSocketProtocol::~WebSocketProtocol() {
	printf("~WebSocketProtocol\n");
}


void WebSocketProtocol::broadcast(const string& msg) {
	string buf(LWS_SEND_BUFFER_PRE_PADDING+1024+LWS_SEND_BUFFER_POST_PADDING, 0);
	unsigned char* p = (unsigned char*)&buf[LWS_SEND_BUFFER_PRE_PADDING];
	memcpy(p, msg.c_str(), msg.size());
	int n = libwebsockets_broadcast(&websockets->ws_protocols[protocol_dx], p, msg.size());
	if(n < 0) {
		printf("Error: cannot broadcast.\n");
	}
}

} // roxlu