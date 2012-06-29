#include "SimpleClient.h"
#include "SimpleServer.h"

// NO POCO, USE LIBEVENT

namespace roxlu {

//SimpleClient::SimpleClient(StreamSocket streamSocket, SocketReactor& socketReactor) 
//	:sock(streamSocket)
//	,reactor(socketReactor)
//{
//	reactor.addEventHandler(sock, NObserver<SimpleClient, ReadableNotification>(*this, &SimpleClient::onReadable));
//	reactor.addEventHandler(sock, NObserver<SimpleClient, ShutdownNotification>(*this, &SimpleClient::onShutdown));
//}

SimpleClient::~SimpleClient() {
//	reactor.removeEventHandler(sock,NObserver<SimpleClient, ReadableNotification>(*this,&SimpleClient::onReadable));
//	reactor.removeEventHandler(sock,NObserver<SimpleClient, ShutdownNotification>(*this,&SimpleClient::onShutdown));
}

//void SimpleClient::onReadable(const AutoPtr<ReadableNotification>& pNotif) {
//	char buf[1024];
//	int read = sock.receiveBytes(buf, 1024);
//	if(read > 0) {
//		read_buffer.storeBytes(buf, read);
//		onDataReceived();
//	}
//	else {
//		delete this;
//	}
//}
//
//void SimpleClient::onShutdown(const AutoPtr<ShutdownNotification>& pNotif) {
//	delete this;
//}


}; // roxlu