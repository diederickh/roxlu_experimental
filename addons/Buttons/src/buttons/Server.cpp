#include <buttons/Server.h>
#include <buttons/Buttons.h>

namespace buttons {

	// CLIENT <--> SERVER BUFFER UTILS
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ClientServerUtils::ClientServerUtils() {
	}

	ClientServerUtils::~ClientServerUtils() {
	}
	
	// Serializes data into a buffer after receiving a value changed event.
	bool ClientServerUtils::serializeOnValueChanged(const Buttons& buttons, const Element* target, ButtonsBuffer& result) {
		unsigned int buttons_id = buttons_hash(buttons.title.c_str(), buttons.title.size());
		unsigned int element_id = buttons_hash(target->label.c_str(), target->label.size());
		bool ret = true;

		// Serialize for each type.
		switch(target->type) {
		case BTYPE_SLIDER: {
			const Sliderf* sliderf = static_cast<const Sliderf*>(target);
			if(sliderf->value_type == Slider<float>::SLIDER_FLOAT) {
				result.addUI32(buttons_id);
				result.addUI32(element_id);
				result.addFloat(sliderf->value);
			};
			break;
		}
		default: {
			printf("Error: unhandled sync type: %d\n", target->type); 
			ret = false;
			break;
		}};

		return ret;
	}

	bool ClientServerUtils::deserializeOnValueChanged(
																	  ButtonsBuffer& buffer
																	  ,CommandData& result
																	  ,std::map<unsigned int, std::map<unsigned int, buttons::Element*> >& elements
																	  ) 
	{
		int command_name = buffer.consumeByte();		


		switch(command_name) {
		case BCLIENT_VALUE_CHANGED: {
			unsigned int command_size = buffer.consumeUI32();
			result.buttons_id = buffer.consumeUI32();
			result.element_id = buffer.consumeUI32();
			result.element = elements[result.buttons_id][result.element_id];

			Sliderf* sliderf = static_cast<Sliderf*>(result.element);
			if(sliderf->value_type == Slider<float>::SLIDER_FLOAT) {
				result.sliderf = sliderf;
				result.sliderf_value = buffer.consumeFloat();
				result.name = BDATA_SLIDERF;
				//printf("Bytes left in buffer: %zu, value: %f\n", buffer.size(), result.sliderf_value);
				return true;
			}
			
			break;
		}
		default: {
			return false;
			break;
		}};
		return false;
	}

	// SERVER CONNECTION
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ServerConnection::ServerConnection(Server& server, ServerConnections& connections, Socket clientSocket)
		:connections(connections)
		,client(clientSocket)
		,server(server)
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

	void ServerConnection::read() {
		client.setBlocking(false);
		std::vector<char> tmp(2048);
		int bytes_read = client.read(&tmp[0], tmp.size());
		if(bytes_read) {
			read_buffer.addBytes((char*)&tmp[0], bytes_read);
			parseReadBuffer();
		}
		client.setBlocking(true);
	}

	// PARSE INCOMING COMMANDS
	void ServerConnection::parseReadBuffer() {
		printf("Recieved data, we have %zu bytes in buffer.\n", read_buffer.size());
		do {
			if(read_buffer.size() < 5) { // minimum size of a command
				return;
			}

			unsigned int command_size = read_buffer.getUI32(1);
			if(read_buffer.size() < command_size) {
				printf("Buffer has only %zu bytes, and the command consists of: %u bytes.\n", read_buffer.size(), command_size);
				return;
			}

			// when we arrive at this point we received a complete command
			CommandData deserialized;
			if(util.deserializeOnValueChanged(read_buffer, deserialized, server.elements)) {
				printf("Correctly deserialized.\n");
				server.addTask(deserialized);
			}

			printf("Command size: %u\n", command_size);
			
		} while(read_buffer.size() > 0);
		printf("Ready parsing read buffer.\n");

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
				// SEND DATA TO CLIENTS OF NECESSARY
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

				// RECEIVE DATA FROM CLIENTS
				for(std::vector<ServerConnection*>::iterator sit = connections.begin(); sit != connections.end(); ++sit) {
					(*sit)->read();
				}
				
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
				ServerConnection* con = new ServerConnection(*this, connections, client);
				connections.addConnection(con);
				sendScheme(con);
			}
		}
	}
	

	void Server::addTask(CommandData cmd) {
		mutex.lock();
		{
			tasks.push_back(cmd);
		}
		mutex.unlock();
	}

	void Server::update() {
		mutex.lock();
		for(std::vector<CommandData>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
			CommandData& cmd = *it;
			switch(cmd.name) {
			case BDATA_SLIDERF: {
				if(cmd.sliderf != NULL) {
					cmd.sliderf->setValue(cmd.sliderf_value);
					cmd.sliderf->needsRedraw();
				}
				break;
			};
			case BDATA_SLIDERI: {
				break;
			}
			default: printf("Error: Server received an Unhandled client task.\n"); break;
			}
		}
		mutex.unlock();

		// and remove all handled tasks.
		tasks.clear();
	}

	void Server::syncButtons(Buttons& buttons) {
		buttons.addListener(this);
		buttons_to_sync.push_back(&buttons);

		// Keep track of all elements for this gui (used when decoding messages from clients)
		unsigned int buttons_id = buttons_hash(buttons.title.c_str(), buttons.title.size());
		for(std::vector<Element*>::iterator it = buttons.elements.begin(); it != buttons.elements.end(); ++it) {
			Element& el = **it;
			unsigned int element_id = buttons_hash(el.label.c_str(), el.label.size());
			elements[buttons_id][element_id] = *it;
		}

		// (Re)create the buttons cheme
		createButtonsScheme();
	}

	void Server::onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target) {
		printf("onEvent: %d = %d\n", event, BEVENT_VALUE_CHANGED);
		if(event == BEVENT_VALUE_CHANGED) {
			ServerCommand cmd(BSERVER_VALUE_CHANGED);
			if(utils.serializeOnValueChanged(buttons, target, cmd.buffer)) {
				connections.sendToAll(cmd);
			}

			/*
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
			*/
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
