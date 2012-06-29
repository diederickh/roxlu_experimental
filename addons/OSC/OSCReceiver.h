#ifndef ROXLU_OSCH
#define ROXLU_OSCH

#include "OSCMessage.h"
#include "OscTypes.h"
#include "OscPacketListener.h"
#include "UdpSocket.h"
#include "pthread.h"
#include <deque>

using std::deque;

namespace roxlu {

class OSCReceiver : public osc::OscPacketListener {
public:

	OSCReceiver(int port);
	~OSCReceiver();
	bool hasMessages();
	bool getNextMessage(OSCMessage* m);
	void ProcessMessage(const osc::ReceivedMessage &m, const IpEndpointName& endpoint);
	
private:
	static void* startThread(void* receiverInstance);
	void shutdown();
	void grabMutex();
	void releaseMutex();

	UdpListeningReceiveSocket* listen_socket;
	deque<OSCMessage*> messages;
	int port;
	bool has_shutdown;
		
	#ifdef TARGET_WIN32
	#else
		pthread_t thread;
		pthread_mutex_t mutex;
	#endif
	

};

}

#endif