#ifndef SIMPLECLIENTH
#define SIMPLECLIENTH

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/NObserver.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/AutoPtr.h"
#include <iostream>
#include "Roxlu.h"

using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Thread;
using Poco::NObserver;
using Poco::AutoPtr;

namespace roxlu {

template<typename C>
class SimpleServer;

class SimpleClient {
public:
	SimpleClient(StreamSocket conSocket, SocketReactor& sockReactor);
	~SimpleClient();
	
	void onReadable(const AutoPtr<ReadableNotification>& pNotif);
	void onShutdown(const AutoPtr<ShutdownNotification>& pNotif);
	
protected:
	virtual void onDataReceived() = 0;
	
	IOBuffer read_buffer;
	StreamSocket sock;
	SocketReactor& reactor;
};

}; // roxlu
#endif