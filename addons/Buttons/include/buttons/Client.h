#ifndef ROXLU_BUTTONS_CLIENTH
#define ROXLU_BUTTONS_CLIENTH

#include <buttons/Types.h>
#include <buttons/Element.h>

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <roxlu/Roxlu.h> 

namespace buttons {

	enum ClientTaskName {
		 BCLIENT_PARSE_SCHEME
		 ,BCLIENT_VALUE_CHANGED_SLIDERI
		 ,BCLIENT_VALUE_CHANGED_SLIDERF
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

	class Client : public roxlu::Runnable {
	public:
		Client(const std::string ip, int port);
		~Client();
		void start();
		void run();
		void update();
		void draw();
	private:
		void parseBuffer();
		void parseCommandScheme();
		void parseCommandTest();
		void parseCommandValueChanged();
		void parseTaskScheme(ClientTask& task);
		void addTask(ClientTask task); // name, ButtonsBuffer buffer);
	private:
		Socket sock;
		roxlu::Thread thread;
		roxlu::Mutex mutex;
		int port;
		std::string ip;
		ButtonsBuffer buffer;
		std::vector<ClientTask> tasks;
		//std::vector<buttons::Buttons*> buttons;
		std::map<unsigned int, buttons::Buttons*> buttons; 
		std::map<unsigned int, std::map<unsigned int, buttons::Element*> > elements;
		// used on guis
		std::vector<float*> value_floats;
	};

} // buttons
#endif
