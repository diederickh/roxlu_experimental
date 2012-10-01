#include <buttons/Server.h>
#include <buttons/Buttons.h>

namespace buttons {

	// SERVER CONNECTION
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ServerConnection::ServerConnection(ServerConnections& connections, Socket clientSocket)
		:connections(connections)
		,client(clientSocket)
	{

	}

	ServerConnection::~ServerConnection() {
		printf("~ServerConnection: remove from connections manager\n");
	}

	bool ServerConnection::send(const char* data, size_t len) {
		size_t todo = len;
		do {
			int done = client.send(data, len);
			if(done < 0) {
				connections.removeConnection(this);
				return false;
			}
			todo -= len;
		} while(todo > 0);
		return true;
	}

	void ServerConnection::send(ServerCommand& cmd) {
		if(!send((const char*)&cmd.name, 1)) {
			return;
		}

		unsigned int size = cmd.buffer.size() * sizeof(char);
		printf(">> Sending to client: %zu\n", size);
		if(!send((const char*)&size, sizeof(size))) {
			return;
		}
		
		if(!send((const char*)&cmd.buffer[0], size)) {
			return;
		}
	}

	// SERVER CONNECTIONS
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ServerConnections::ServerConnections(Server& server)
		:server(server)
	{
	}

	ServerConnections::~ServerConnections() {
		printf("~ServerConnections: @todo close client sockets!\n");
	}

	void ServerConnections::start() {
		thread.create(*this);
	}

	void ServerConnections::run() {
		while(true) {
			mutex.lock();
			{
				for(std::deque<ServerCommand>::iterator it = commands.begin(); it != commands.end(); ++it) {
					for(std::vector<ServerConnection*>::iterator sit = connections.begin(); sit != connections.end(); ++sit) {
						(*sit)->send((*it));
					}
					std::vector<ServerConnection*>::iterator rit = remove_list.begin();
					while(rit != remove_list.end()) {
						std::vector<ServerConnection*>::iterator fit = std::find(connections.begin(), connections.end(), (*rit));
						if(fit != connections.end()) {
							delete (*fit);
							connections.erase(fit);
						}
						++rit;
					}
					remove_list.clear();
				}
				commands.clear();
			}
			mutex.unlock();
		}
	}

	void ServerConnections::sendToAll(ButtonServerCommandName name, const char* buffer, size_t len) {
		ServerCommand cmd(name);
		cmd.setData(buffer, len);
		addCommand(cmd);
	}

	void ServerConnections::sendToAll(ServerCommand& cmd) {
		addCommand(cmd);
	}

	void ServerConnections::addConnection(ServerConnection* conn) {
		mutex.lock();
		{
			connections.push_back(conn);
		}
		mutex.unlock();
	}
	
	void ServerConnections::addCommand(ServerCommand cmd) {
		mutex.lock();
		{
			commands.push_back(cmd);
		}
		mutex.unlock();
	}

	void ServerConnections::removeConnection(ServerConnection* conn) {
		// @todo this is indirectly called by "send()
		remove_list.push_back(conn);
	}

	// SERVER 
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Server::Server(const std::string ip, int port)
		:ip(ip)
		,port(port)
		,connections(*this)
	{
	}

	Server::~Server() {
	}

	void Server::start() {
		thread.create(*this);
	}

	void Server::run() {
		if(!server_socket.create()) {
			printf("Error: cannot create socket.\n");
			::exit(0);
		}
		if(!server_socket.setReUseAddress()) {
			printf("Error: cannot set the reuse socket option.\n");
			::exit(0);
		}
		if(!server_socket.bind(ip.c_str(), port)) {
			::exit(0);
		}
		if(!server_socket.listen(5)) {
			::exit(0);
		}
		server_socket.setNoSigPipe();
		connections.start(); // separate thread to handle clients.

		while(true) {
			Socket client;
			if(server_socket.accept(client)) {
				client.setNoSigPipe();
				ServerConnection* con = new ServerConnection(connections, client);
				connections.addConnection(con);
				sendScheme(con);
			}
		}
	}
	
	void Server::syncButtons(Buttons& buttons) {
		buttons.addListener(this);
		buttons_to_sync.push_back(&buttons);
		createButtonsScheme();
	}

	void Server::onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target) {
		printf("onEvent: %d = %d\n", event, BEVENT_VALUE_CHANGED);
		if(event == BEVENT_VALUE_CHANGED) {
			unsigned int buttons_id = buttons_hash(buttons.title.c_str(), buttons.title.size());
			unsigned int element_id = buttons_hash(target->label.c_str(), target->label.size());
			switch(target->type) {
			case BTYPE_SLIDER: {
				const Sliderf* sliderf = static_cast<const Sliderf*>(target);
				if(sliderf->value_type == Slider<float>::SLIDER_FLOAT) {
					ServerCommand cmd(BSERVER_VALUE_CHANGED);
					cmd.buffer.addUI32(buttons_id);
					cmd.buffer.addUI32(element_id);
					cmd.buffer.addFloat(sliderf->value);
					connections.sendToAll(cmd);
					printf("Send: %u %u \n", buttons_id, element_id);
				};
				break;
			}
			default: printf("Error: unhandled sync type: %d\n", target->type); break;
			};
		}
	}
	
	// Create the scheme which is used to recreate the buttons gui on the other side.
	void Server::createButtonsScheme() {
		scheme.clear();
		scheme.addByte(BSERVER_SCHEME);

		// Reserve space for the size of the scheme
		unsigned int scheme_size = 0;
		int start = scheme.size();
		scheme.addUI32(scheme_size);

		// first all buttons which are not part of a panel
		for(std::vector<Buttons*>::iterator it = buttons_to_sync.begin(); it != buttons_to_sync.end(); ++it) {
			Buttons& b = **it;
			// >> general info
			scheme.addByte(BSCHEME_GUI);
			scheme.addUI32(b.x);
			scheme.addUI32(b.y);
			scheme.addUI32(b.w);
			scheme.addUI32(b.h);
			scheme.addString(b.title);
			
			// >> elements
			scheme.addUI16(b.elements.size());
			for(int i = 0; i < b.elements.size(); ++i) {
				Element* el = b.elements[i];
				scheme.addByte(el->type);
				scheme.addString(el->label);
				scheme.addFloat(el->col_hue);
				scheme.addFloat(el->col_sat);
				scheme.addFloat(el->col_bright);

				switch(el->type) {
				case BTYPE_SLIDER: {
					Sliderf* sliderf = static_cast<Sliderf*>(el);
					scheme.addByte(sliderf->value_type);
					if(sliderf->value_type == Slider<float>::SLIDER_FLOAT) {
						scheme.addFloat(sliderf->value);
						scheme.addFloat(sliderf->minv);
						scheme.addFloat(sliderf->maxv);
						scheme.addFloat(sliderf->stepv);
					}
					else {
					}
					break;
				}
				default:break;
				};
			}
		}

		// rewrite the size of the scheme
		int end = scheme.size();
		int num_bytes = end - start;
		scheme.rewrite(start, 4, (char*)&num_bytes);

		printf("Num bytes of scheme: %d, in buffer: %zu\n", num_bytes, scheme.size());
		printf("Size of int :%d\n", sizeof(unsigned int));
	}

	void Server::sendScheme(ServerConnection* con) {
		con->send(scheme.getPtr(), scheme.getNumBytes());
	}
	
	void Server::testSend() {
		std::string test_data = "hello world.\n";
		connections.sendToAll(BSERVER_COMMAND_TEST, test_data.c_str(), test_data.size()); 
	}

	
} // buttons
