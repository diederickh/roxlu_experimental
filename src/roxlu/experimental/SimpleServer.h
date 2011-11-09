#ifndef SIMPLESERVERH
#define SIMPLESERVERH

/**
 * Very simple, fast TCP server implementation which can handles hundreds
 * of simultanious connected clients. To use this, you extends the SimpleClient
 * and inherit some abstract member functions which are called automagically.
 * 
 * Example: Extending the simple client
 * -------------------------------------
	 class iPhoneClient : public SimpleClient {
		public:	
			iPhoneClient(StreamSocket conSocket, SocketReactor& sockReactor)
				:SimpleClient(conSocket, sockReactor)
			{
			}
			

		protected: 
			virtual void onDataReceived();
	};
 * -------------------------------------
 *
 * For each new client an instance of this class is created and onDataReceived
 * is called when we read new data into the inherited member "read_buffer".
 *
 * To create an instance of this simple server use this:
 * ----------------------------------------------------
 * SimpleServer<iPhoneClient> server;
 * server.setup(1234); // set port
 * server.start();
 */

#include "Poco/Thread.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/socketAddress.h"
#include "Poco/Mutex.h"
#include "Poco/Runnable.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"

#include <vector>
#include <string>

using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Thread;
using Poco::Mutex;
using Poco::Runnable;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::IPAddress;
using Poco::NObserver;
using Poco::AutoPtr;
using std::string;
using std::vector;

namespace roxlu {

template <typename C>
class SimpleAcceptor;

// TCP server
// --------------------
template <typename T>
class SimpleServer {
public:
	typedef T SimpleServerType;
	
	SimpleServer() 
		:server_port(0)
		,server_socket(NULL)
		,server_reactor(NULL)
		,server_address(NULL)
	{
	}
	

	~SimpleServer() {
	}
	
	void setup(int port) {
		server_port = port;
	}
	
	void start() {
		if(server_socket != NULL) {
			printf("SimpleServer: error, already created a socket.\n");
			return;
		}
		
		IPAddress wildcard_address;
		server_address = new SocketAddress(wildcard_address, server_port);
		server_socket = new ServerSocket(*server_address);
		server_reactor = new SocketReactor();
		server_acceptor = new SimpleAcceptor<T>(*server_socket, *server_reactor, this);
		
		server_reactor->addEventHandler(*server_socket, NObserver<SimpleServer, ReadableNotification>(*this, &SimpleServer::onReadable));
		server_reactor->addEventHandler(*server_socket, NObserver<SimpleServer, ShutdownNotification>(*this, &SimpleServer::onShutdown));
		
		reactor_thread = new Thread();
		reactor_thread->start(*server_reactor);
	}
	
	void onReadable(const AutoPtr<ReadableNotification>& pNotif) {
	}

	void onShutdown(const AutoPtr<ShutdownNotification>& pNotif) {
		printf("onShutdown socket reactor.\n");
	}

	void addClient(T* client) {
		mutex.lock();
		clients.push_back(client);
		mutex.unlock();
	}
	
	void removeClient(T* client) {
		mutex.lock();
		typename vector<T*>::iterator it = std::find(clients.begin(), clients.end(), client);
		if(it != clients.end()) {
			clients.erase(it);
			printf("SimpleServer: removed client.\n");
		}
		mutex.unlock();

	}
	
	vector<T*>& getClients() {
		return clients;
	}
	
private:
	int server_port;
	ServerSocket* server_socket;
	SocketReactor* server_reactor;
	SocketAddress* server_address;
	Mutex mutex;
	Thread* reactor_thread;
	SocketAcceptor<T>* server_acceptor;
	vector<T*> clients;
};


// connection acceptor
// --------------------
template <typename T>
class SimpleAcceptor : public SocketAcceptor<T> {
public:
	SimpleAcceptor(ServerSocket& socket, SocketReactor& reactor, SimpleServer<T>* server)
		:SocketAcceptor<T>(socket, reactor)
		,simple_server(server)
	{
	}
	SimpleServer<T>* simple_server;
protected:
		virtual T* createServiceHandler(StreamSocket& sock) {
			T* obj = new T(sock, *(SocketAcceptor<T>::reactor()));
			simple_server->addClient(obj);
			return obj;
	}
};

}; // roxlu

#endif
