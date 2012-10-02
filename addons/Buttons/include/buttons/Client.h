#ifndef ROXLU_BUTTONS_CLIENTH
#define ROXLU_BUTTONS_CLIENTH

#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Server.h>

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <roxlu/Roxlu.h> 

namespace buttons {

	enum ClientTaskName {
		 BCLIENT_PARSE_SCHEME
		 ,BCLIENT_SEND_TO_SERVER
		 ,BCLIENT_VALUE_CHANGED_SLIDERI
		 ,BCLIENT_VALUE_CHANGED_SLIDERF
		 //		 ,BCLIENT_VALUE_CHANGED
	};

	struct ClientTask {
		ClientTask(ClientTaskName name)
			:name(name)
			,sliderf_value(0.0f)
			,slideri_value(0)
			,element(NULL)
			,sliderf(NULL)
			,buttons(NULL)
		{
		}

		ClientTaskName name;
		ButtonsBuffer buffer;
		Element* element;
		Sliderf* sliderf;
		Buttons* buttons;

		float sliderf_value;
		int slideri_value;
	};
	/*	
	class ClientConnection : public roxlu::Runnable {
	public:
		void addTask(ClientTask task);
		void run();
	private:
		std::vector<ClientTask> out_tasks; // used for Client --> Server communication
	};
	*/
	class Client : public roxlu::Runnable, public ButtonsListener {
	public:
		Client(const std::string ip, int port);
		~Client();
		void start();
		void run();
		void update();
		void draw();

		void onMouseDown(int x, int y);
		void onMouseUp(int x, int y);
		void onMouseMoved(int x, int y);

		void onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target);

	private:
		void parseBuffer();
		//		void parseCommandScheme();
		void parseScheme(CommandData& cmd);
		void parseCommandTest();
		void parseCommandValueChanged();
		//		void parseTaskScheme(ClientTask& task);
		//void addInTask(ClientTask task); // name, ButtonsBuffer buffer);
		//void addOutTask(CommandData data);

		void addInCommand(CommandData task);
		void addOutCommand(CommandData task); // will be name: addSendTask or something.
		void addSendToServerTask(CommandDataName name, ButtonsBuffer buffer); // adds a new task to the server; sending is done in a separate thread
		void send(const char* buffer, size_t len); // send data to server
	private:
		Socket sock;
		roxlu::Thread thread;
		roxlu::Mutex mutex;
		int port;
		std::string ip;
		ButtonsBuffer buffer;
		ClientServerUtils util;
		//std::vector<CommandData> tasks;
		//std::vector<ClientTask> in_tasks; // ussed for Server --> Client comunication
		std::vector<CommandData> out_commands; // used for Client --> Server communication
		std::vector<CommandData> in_commands; // must be handle in own thread
		std::map<unsigned int, buttons::Buttons*> buttons; 
		std::map<unsigned int, std::map<unsigned int, buttons::Element*> > elements;
		// used on guis
		std::vector<float*> value_floats;

	};

} // buttons
#endif
