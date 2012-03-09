#include "WebSocketConnection.h"
#include "WebSockets.h"
#include "WebSocketProtocol.h"

namespace roxlu {

// TODO: implement binary
WebSocketConnection::WebSocketConnection(WebSockets* websockets, WebSocketProtocol* protocol)
	:websockets(websockets)
	,protocol(protocol)
	,ws(NULL)
	,buffer(LWS_SEND_BUFFER_PRE_PADDING+1024+LWS_SEND_BUFFER_POST_PADDING)
{
}

WebSocketConnection::~WebSocketConnection() {
	printf("~WebSocketConnection\n");
}

void WebSocketConnection::close() {
	websockets->close(this);
}

void WebSocketConnection::send(const string& msg) {
	int n = 0;
	unsigned char* p = &buffer[LWS_SEND_BUFFER_PRE_PADDING];
	memcpy(p, msg.c_str(), msg.size());
	
	n = libwebsocket_write(ws, p, msg.size(), LWS_WRITE_TEXT);
	if(n < 0) {
		printf("Error: cannot write to socket.\n");
	}
}


const string WebSocketConnection::receive(const string& msg) {
	// TODO: handle binary
	return msg;
}

} // roxlu