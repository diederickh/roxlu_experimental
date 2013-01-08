#ifndef ROXLU_WEBSOCKETSH
#define ROXLU_WEBSOCKETSH

#include <string>
#include <vector>
#include "WebSocketProtocol.h"
#include "WebSocketConnection.h"

extern "C" {
 #include "libwebsockets.h"
}

using std::string;
using std::vector;
using std::pair;

namespace roxlu {


class WebSockets {
public:
	WebSockets();
	~WebSockets();
	static WebSockets* getInstance();
	void start(const unsigned int port);
	void update();
	void close(WebSocketConnection* conn);
	
	WebSocketProtocol* getProtocol(const unsigned int dx);
	void addProtocol(const string& name, WebSocketProtocol& protocol);

	unsigned int onCallback(
			 WebSocketConnection* conn
			,enum libwebsocket_callback_reasons const reason
			,const char* message
			,const unsigned int len
	);
	
	unsigned int onAllow(
			WebSocketProtocol* protocol
			,const long fd
	);
	

	static WebSockets* instance;
	vector<struct libwebsocket_protocols> ws_protocols;
	vector<pair<string, WebSocketProtocol*> > protocols;
	struct libwebsocket_context* ws_context;	
	string interface;
};

inline WebSocketProtocol* WebSockets::getProtocol(const unsigned int dx) {
	if(dx > protocols.size()) {
		return NULL;
	}
	return protocols[dx].second;
}

inline WebSockets* WebSockets::getInstance() {
	return instance;
}

extern "C" {

int ws_callback(	 struct libwebsocket_context *context
 				       	,struct libwebsocket *ws
						,enum libwebsocket_callback_reasons reason
						,void *user
						,void *_message
						,size_t len
					
);

}
} // roxlu

#endif