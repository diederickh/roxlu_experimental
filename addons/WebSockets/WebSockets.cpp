#include "WebSockets.h"

namespace roxlu {

WebSockets* WebSockets::instance = NULL;

WebSockets::WebSockets() 
	:ws_context(NULL)
{
	WebSockets::instance = this;
}

WebSockets::~WebSockets() {
	if(ws_context != NULL) {
		libwebsocket_context_destroy(ws_context);
	}
}

void WebSockets::start(const unsigned int port) {
	printf("Start...\n");
	
	// create websocket array
	vector<pair<string, WebSocketProtocol*> >::iterator it = protocols.begin();
	while(it != protocols.end()) {
		pair<string, WebSocketProtocol*>& p = *it;
		struct libwebsocket_protocols wsp = {	
							 p.second->getName().c_str()
							,ws_callback
							,sizeof(WebSocketConnection)
		};
		ws_protocols.push_back(wsp);
		++it;
	}
	struct libwebsocket_protocols end_protocol = {NULL, NULL, 0};
	ws_protocols.push_back(end_protocol);
	
	// create the websocket context
	ws_context = libwebsocket_create_context(
							 port
							,interface.c_str()
							,&ws_protocols[0]
							,libwebsocket_internal_extensions
							,NULL, NULL
							,-1, -1
							,0
	);
	
	if(ws_context == NULL) {
		printf("Cannot create websocket context");
		exit(0);
	}
}


void WebSockets::update() {
	libwebsocket_service(ws_context,1);
}

void WebSockets::addProtocol(const string& name, WebSocketProtocol& protocol) {
	protocol.setName(name);
	protocol.setProtocolIndex(protocols.size());
	protocol.websockets = this;
	protocols.push_back(make_pair(name, &protocol));
}

void WebSockets::close(WebSocketConnection* conn) {
	if(conn == NULL || conn->ws == NULL) {
		return;
	}
	printf("Close connections...\n");
	libwebsocket_close_and_free_session(ws_context, conn->ws, LWS_CLOSE_STATUS_NORMAL);
}

unsigned int WebSockets::onAllow(
			WebSocketProtocol* protocol
			,const long fd
)
{
	string client_ip(128, 0);
	string client_name(128, 0);
	
	libwebsockets_get_peer_addresses(
			(int)fd
			,&client_name[0]
			,client_name.size()
			,&client_ip[0]
			,client_ip.size()
	);
	
	return protocol->allowClient(client_name, client_ip);
}


unsigned int WebSockets::onCallback(
					 WebSocketConnection* conn
					,enum libwebsocket_callback_reasons const reason
					,const char* message
					,const unsigned int len
)
{
	if(conn == NULL || conn->protocol == NULL) {
		return 1;
	}
	
	string msg;
	if(message != NULL && len > 0) {
		msg = string(message, len);
	}
	
	switch(reason) {
		case LWS_CALLBACK_ESTABLISHED: {
			conn->protocol->onOpen(msg, *conn);
			break;
		}
		case LWS_CALLBACK_CLOSED: {
			conn->protocol->onClose(msg, *conn);
			break;
		}
		case LWS_CALLBACK_SERVER_WRITEABLE: {
			conn->protocol->onIdle(msg, *conn);
			break;
		}	
		case LWS_CALLBACK_BROADCAST: {
			conn->protocol->onBroadcast(msg, *conn);
			break;
		}
		case LWS_CALLBACK_RECEIVE: {
			conn->protocol->onMessage(msg, *conn);
			break;
		}
		
		default:break;
	}
	return 0;	
}


} // roxlu  


extern "C" {

int ws_callback(	
		 struct libwebsocket_context *context
 		,struct libwebsocket *ws
		,enum libwebsocket_callback_reasons reason
		,void *user
		,void *message
		,size_t len
)
{
	roxlu::WebSockets* websockets = roxlu::WebSockets::getInstance();
	
	// get protocol handle.
	const struct libwebsocket_protocols* ws_protocol = libwebsockets_get_protocol(ws);
	int dx = (ws_protocol) ? ws_protocol->protocol_index : 0;
	roxlu::WebSocketProtocol* protocol = websockets->getProtocol(dx);
	
	// connection
	roxlu::WebSocketConnection** const conn_ptr = (roxlu::WebSocketConnection**)user;
	roxlu::WebSocketConnection* conn;
	
	// connection established & closed
	if(reason == LWS_CALLBACK_ESTABLISHED) {
		*conn_ptr = new roxlu::WebSocketConnection(websockets, protocol);
	}
	else if(reason == LWS_CALLBACK_CLOSED) {
		if(*conn_ptr != NULL) {
			delete *conn_ptr;
		}
	}
	
	switch(reason) {
		case LWS_CALLBACK_FILTER_NETWORK_CONNECTION: {
			if(protocol != NULL) {
				return websockets->onAllow(protocol, (int)(long)user) ? 0 : 1;
			}
			else {
				return 0;
			}
		};
		
		case LWS_CALLBACK_ESTABLISHED:
		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_SERVER_WRITEABLE:
		case LWS_CALLBACK_RECEIVE:
		case LWS_CALLBACK_BROADCAST: {
			conn = *(roxlu::WebSocketConnection**)user;
			if(conn && conn->ws != ws) {
				conn->ws = ws;
			}
			return websockets->onCallback(conn, reason, (char*)message, len);
		};
		default:return 0;
	};
	
	return 1;
}


} // extern C