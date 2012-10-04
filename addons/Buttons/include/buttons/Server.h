#ifndef ROXLU_BUTTONS_SERVERH
#define ROXLU_BUTTONS_SERVERH

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Slider.h>
#include <buttons/Toggle.h>
#include <buttons/Button.h>
#include <buttons/Radio.h>
#include <buttons/Pad.h>

#include <string>
#include <vector>
#include <deque>
#include <roxlu/Roxlu.h> 

namespace buttons {
	class Server;
	class ServerConnections;
	
	// Structure which is used to hold deserialized data for gui types
	enum CommandDataName {
		BDATA_SCHEME // scheme: following data contains gui definitions
		,BDATA_GUI // scheme: flag there comes a gui definition
		,BDATA_CHANGED // value change on client or server
		,BDATA_SLIDERF // contains slider float
		,BDATA_SLIDERI  // contains slider integer
		,BDATA_TOGGLE // contains toggle boolean value
		,BDATA_BUTTON // contains button data
		,BDATA_RADIO // contains the selected index of a radio
		,BDATA_COLOR // color data
		,BDATA_VECTOR // vector (float 2) data
		,BDATA_PADF // float pad value (2 floats, x/y percentage)
		,BDATA_PADI // int pad value (2 floats, x/y percentage)
	};

	// "All-valude" datatype for handling gui changes.
	struct CommandData {
		CommandData() 
			:element(NULL)
			,buttons(NULL)
			,sliderf(NULL)
			,slideri(NULL)
			,toggle(NULL)

			,sliderf_value(0.0f)
			,slideri_value(0)
			,buttons_id(0)
			,element_id(0)
			,toggle_value(false)
			,button_value(0)
			,radio_value(0)
		{
			memset(color_value, 0, 4 * sizeof(unsigned int));
			memset(vector_value, 0, 2 * sizeof(float));
			memset(pad_value, 0, 2 * sizeof(float));
		}

		ButtonsBuffer buffer;
		CommandDataName name;
		Element* element;

		Buttons* buttons;
		Sliderf* sliderf;
		Slideri* slideri;
		Toggle* toggle;

		bool toggle_value;
		float sliderf_value;
		int slideri_value;
		int button_value;
		int radio_value; // selected index
		unsigned int color_value[4];
		float vector_value[2];
		float pad_value[2]; // x, y percentage

		unsigned int buttons_id;
		unsigned int element_id;
	};

	// Utils for creating buffers which are send from client <--> server
	class ClientServerUtils {
	public:
		ClientServerUtils();
		~ClientServerUtils();
		bool serialize(const Buttons& buttons, const Element* target, ButtonsBuffer& result, void* targetData);
		bool deserialize(ButtonsBuffer& buffer, CommandData& data, std::map<unsigned int, std::map<unsigned int, Element*> >& elements);
	};

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
		ServerConnection(Server& server, ServerConnections& connections, Socket clientSocket);
		~ServerConnection();
		void send(ServerCommand& cmd);
		bool send(const char* data, size_t len);
		void read();
		void parseBuffer();
	public:
		Server& server; // we use server.elements when parsing client data; could be thread buggy
	private:
		ServerConnections& connections;
		ButtonsBuffer buffer; // buffer with data from client
		Socket client;
		ClientServerUtils util;
	};

	class ServerConnections : public roxlu::Runnable {
	public:
		ServerConnections(Server& server);
		~ServerConnections();
		void start();
		void run(); // thread
		void addConnection(ServerConnection* con);
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
		void update();
		void syncButtons(Buttons& buttons);
		void onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target, void* targetData);
		void addTask(CommandData cmd);
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
		ClientServerUtils utils; // helper for client/server to generate buffers
		std::vector<CommandData> tasks; // list of received and parsed commands we need to handle (e.g. change gui values)
	public:
		std::map<unsigned int, std::map<unsigned int, buttons::Element*> > elements; // indexed by buttons-hash and element hash
	};

} // buttons
#endif
