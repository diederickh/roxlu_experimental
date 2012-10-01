#ifndef ROXLU_BUTTONS_SERVERH
#define ROXLU_BUTTONS_SERVERH

#include <buttons/Types.h>
#include <buttons/Element.h>

#include <string>
#include <vector>
#include <deque>
#include <roxlu/Roxlu.h> 

namespace buttons {
	class Server;
	class ServerConnections;

	

	struct ServerCommand {
		ServerCommand(char name):name(name) {
		}

		void setData(const char* data, size_t len) {
			buffer.addBytes((char*)data,len);
		}

		char name;
		ButtonsBuffer buffer;
	};

	class ServerConnection {
	public:
		ServerConnection(ServerConnections& connections, Socket clientSocket);
		~ServerConnection();
		void send(ServerCommand& cmd);
		bool send(const char* data, size_t len);
	private:
		ServerConnections& connections;
		Socket client;
	};

	class ServerConnections : public roxlu::Runnable {
	public:
		ServerConnections(Server& server);
		~ServerConnections();
		void start();
		void run(); // thread
		void addConnection(ServerConnection* con);
		void sendToAll(ButtonServerCommandName name, const char* buffer, size_t len);
		void sendToAll(ServerCommand& cmd);
		void addCommand(ServerCommand cmd);
		void removeConnection(ServerConnection* con);
	private:
		Server& server;
		roxlu::Thread thread;
		roxlu::Mutex mutex;
		std::vector<ServerConnection*> connections;
		std::deque<ServerCommand> commands;
		std::vector<ServerConnection*> remove_list;
	};

	class Server : public roxlu::Runnable, public ButtonsListener {
	public:
		Server(const std::string ip, int port);
		~Server();
		void start();
		void run();
		void syncButtons(Buttons& buttons);
		void onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target);
		void testSend();
	private:
		void createButtonsScheme();
		void sendScheme(ServerConnection* con); // sends all information about the guis to the client so the client can rebuild it.
		
	private:
		Socket server_socket;
		roxlu::Thread thread;
		roxlu::Mutex mutex;
		ServerConnections connections;
		int port;
		std::string ip;
		std::vector<Buttons*> buttons_to_sync;
		ButtonsBuffer scheme; // description used to recreate the panels + buttons on the client side
	};

} // buttons
#endif
