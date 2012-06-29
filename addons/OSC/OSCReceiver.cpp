#include "OSCReceiver.h"
namespace roxlu {

OSCReceiver::OSCReceiver(int port)
	:port(port)
	,listen_socket(NULL)
{
	#ifdef TARGET_WIN32
	#else
		pthread_mutex_init(&mutex, NULL);
	#endif
	
	has_shutdown = false;
	listen_socket = new UdpListeningReceiveSocket(
							IpEndpointName(
								IpEndpointName::ANY_ADDRESS
								,port
							), this
	);
					
	
	pthread_create(&thread, NULL, &OSCReceiver::startThread, (void*)this);
}

OSCReceiver::~OSCReceiver() {
	shutdown();
}

bool OSCReceiver::hasMessages() {	
	grabMutex();
	size_t len = messages.size();
	releaseMutex();
	return len > 0;
}

bool OSCReceiver::getNextMessage(OSCMessage* m) {
	grabMutex();
	if(messages.size() == 0) {
		releaseMutex();
		return false;
	}
	OSCMessage* msg = messages.front();
	m->copy(*msg);

	delete msg;
	messages.pop_front();
	releaseMutex();
	
	return true;
}

void OSCReceiver::shutdown() {
	if(!listen_socket) {
		return;
	}
	listen_socket->AsynchronousBreak();
	while(!has_shutdown) {
		#ifdef TARGET_WIN32 
			Sleep(1);
		#else
			usleep(100);
		#endif
	}
	
	#ifdef TARGET_WIN32 
		ReleaseMutex(mutex);
	#else
		pthread_mutex_destroy(&mutex);
	#endif
	
	delete listen_socket;
	listen_socket = NULL;
}

void* OSCReceiver::startThread(void* receiverInstance) {
	OSCReceiver* instance = static_cast<OSCReceiver*>(receiverInstance);
	instance->listen_socket->Run();
	instance->has_shutdown = true;
	return NULL;
}

void OSCReceiver::ProcessMessage(
	 const osc::ReceivedMessage &m
	,const IpEndpointName& endpoint 
)
{

	OSCMessage* oscm = new OSCMessage();
	oscm->setAddress(m.AddressPattern());
	
	char endpoint_host[IpEndpointName::ADDRESS_STRING_LENGTH];
	endpoint.AddressAsString(endpoint_host);
	oscm->setRemoteEndpoint(endpoint_host, endpoint.port);
		
	osc::ReceivedMessage::const_iterator it = m.ArgumentsBegin();
	while(it != m.ArgumentsEnd()) {
		if(it->IsInt32()) {
			oscm->addInt32(it->AsInt32Unchecked());
		}
		else if(it->IsFloat()) {
			oscm->addFloat(it->AsFloatUnchecked());
		}
		else if(it->IsString()) {
			oscm->addString(it->AsStringUnchecked());
		}
		else {
			printf("warning: unhandled received type.\n");
		}
		++it;
	}
		
	grabMutex();
		messages.push_back(oscm);
	releaseMutex();
}

#ifdef TARGET_WIN32
	void OSCReceiver::grabMutex() {
	}

	void OSCReceiver::releaseMutex() {
	}
#else 

	void OSCReceiver::grabMutex() {
		pthread_mutex_lock(&mutex);
	}

	void OSCReceiver::releaseMutex() {
		pthread_mutex_unlock(&mutex);
	}

#endif
} // roxlu