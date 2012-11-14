#include <buttons/Server.h>
#include <buttons/Buttons.h>
#include <sstream>

namespace buttons {

  // CLIENT <--> SERVER BUFFER UTILS
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ClientServerUtils::ClientServerUtils() {
  }

  ClientServerUtils::~ClientServerUtils() {
  }
	
  // Serializes data into a buffer after receiving a value changed event.
  bool ClientServerUtils::serialize(const Buttons& buttons, const Element* target, ButtonsBuffer& result, void* targetData) {
    unsigned int buttons_id = buttons_hash(buttons.title.c_str(), buttons.title.size());
    unsigned int element_id = buttons_hash(target->label.c_str(), target->label.size());
    bool ret = true;

    // Serialize for each type.
    switch(target->type) {
    case BTYPE_SLIDER: {
      const Sliderf* sliderf = static_cast<const Sliderf*>(target);
      if(sliderf->value_type == Slider<float>::SLIDER_FLOAT) {
        // float slider
        result.addUI32(buttons_id);
        result.addUI32(element_id);
        result.addFloat(sliderf->value);
      }
      else {
        // int slider
        const Slideri* slideri = static_cast<const Slideri*>(target);
        result.addUI32(buttons_id);
        result.addUI32(element_id);
        result.addI32(slideri->value);
      }
      break;
    }
    case BTYPE_TOGGLE: {
      // bool toggle
      const Toggle* toggle = static_cast<const Toggle*>(target);
      result.addUI32(buttons_id);
      result.addUI32(element_id);
      result.addByte(toggle->value == true ? 1 : 0);
      break;
    }
    case BTYPE_BUTTON: {
      // button
      int* button_id = (int*)targetData;
      result.addUI32(buttons_id);
      result.addUI32(element_id);
      result.addUI32(*button_id);
      break;
    }
    case BTYPE_RADIO: {
      // radio
      int* selected = (int*)targetData;
      result.addUI32(buttons_id);
      result.addUI32(element_id);
      result.addUI32(*selected);
      break;
    }
    case BTYPE_COLOR: {
      // color 
      unsigned int* col = (unsigned int*)targetData; // H,S,L,A
      result.addUI32(buttons_id);
      result.addUI32(element_id);
      result.addUI32(col[0]);
      result.addUI32(col[1]);
      result.addUI32(col[2]);
      result.addUI32(col[3]);
      break;
    }
    case BTYPE_VECTOR: {
      // vector
      float* vec = (float*)targetData;
      result.addUI32(buttons_id);
      result.addUI32(element_id);
      result.addFloat(vec[0]);
      result.addFloat(vec[1]);
      break;
    }
    case BTYPE_PAD: {
      result.addUI32(buttons_id);
      result.addUI32(element_id);

      const Pad<float>* padf = static_cast<const Pad<float>* >(target);
      if(padf->value_type == PAD_FLOAT) {
        result.addByte(PAD_FLOAT);
        result.addFloat(padf->px);
        result.addFloat(padf->py);
      }
      else {
        const Pad<int>* padi = static_cast<const Pad<int>* >(target);
        result.addByte(PAD_INT);
        result.addFloat(padi->px);
        result.addFloat(padi->py);
      }
      break;
    }
    default: {
      printf("Error: unhandled sync type: %d\n", target->type); 
      ret = false;
      break;
    }};

    return ret;
  }

  bool ClientServerUtils::deserialize(
                                      ButtonsBuffer& buffer
                                      ,CommandData& result
                                      ,std::map<unsigned int, std::map<unsigned int, buttons::Element*> >& elements
                                      ) 
  {
    int command_name = buffer.consumeByte();		


    switch(command_name) {
    case BDATA_CHANGED: {
      unsigned int command_size = buffer.consumeUI32();
      result.buttons_id = buffer.consumeUI32();
      result.element_id = buffer.consumeUI32();
      result.element = elements[result.buttons_id][result.element_id];
      printf("Deserialize: buttonsid: %u, elementsid: %u, el ptr: %p\n", result.buttons_id, result.element_id, result.element);
      switch(result.element->type) {
      case BTYPE_SLIDER: { 
        Sliderf* sliderf = static_cast<Sliderf*>(result.element);
        if(sliderf->value_type == Slider<float>::SLIDER_FLOAT) {
          // float flider
          result.sliderf = sliderf;
          result.sliderf_value = buffer.consumeFloat();
          result.name = BDATA_SLIDERF;
          return true;
        }
        else {
          // int slider
          Slideri* slideri = static_cast<Slideri*>(result.element);
          result.slideri = slideri;
          result.slideri_value = buffer.consumeI32();
          result.name = BDATA_SLIDERI;
          return true;
        }
        break;
      }
      case BTYPE_TOGGLE: {
        // bool toggle
        Toggle* toggle = static_cast<Toggle*>(result.element);
        result.toggle = toggle;
        result.toggle_value = (buffer.consumeByte() == 1);
        result.name = BDATA_TOGGLE;
        return true;
      }
      case BTYPE_BUTTON: {
        // button
        result.button_value = buffer.consumeUI32();
        result.name = BDATA_BUTTON;
        return true;
      }
      case BTYPE_RADIO: {
        // radio
        result.radio_value = buffer.consumeUI32();
        result.name = BDATA_RADIO;
        return true;
      }
      case BTYPE_COLOR: {
        // color
        result.name = BDATA_COLOR;
        result.color_value[0] = buffer.consumeUI32();
        result.color_value[1] = buffer.consumeUI32();
        result.color_value[2] = buffer.consumeUI32();
        result.color_value[3] = buffer.consumeUI32();
        return true;
      }
      case BTYPE_VECTOR: {
        // vector
        result.name = BDATA_VECTOR;
        result.vector_value[0] = buffer.consumeFloat();
        result.vector_value[1] = buffer.consumeFloat();
        return true;
      }
      case BTYPE_PAD: {
        // pad
        char type = buffer.consumeByte();
        if(type == PAD_FLOAT) {
          result.name = BDATA_PADF;
        }
        else {
          result.name = BDATA_PADI;
        }
        result.pad_value[0] = buffer.consumeFloat();
        result.pad_value[1] = buffer.consumeFloat();
        return true;
      }
      default:break;
      }
      break;
    }
    case BDATA_GET_SCHEME: {
      printf("Client wants to get the scheme.\n");
      result.name = BDATA_GET_SCHEME;
      buffer.consumeUI32(); // contains an "empty" 
      return true;
    }
    case BDATA_SCHEME: {
      unsigned int command_size = buffer.consumeUI32();
      result.name = BDATA_SCHEME;
      result.buffer.addBytes(buffer.getPtr(), buffer.getNumBytes());
      buffer.flush(command_size);
      return true;
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
    printf(">> Sending to client: %d bytes\n", size);
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

    if(bytes_read > 0) {
      printf("Read: %d\n", bytes_read);
      buffer.addBytes((char*)&tmp[0], bytes_read);
      parseBuffer();
    }
    else if(bytes_read < 0) {
      // disconnected
      connections.removeConnection(this);
      return ; 
    }
	
    client.setBlocking(true);
  }

  // PARSE INCOMING COMMANDS
  void ServerConnection::parseBuffer() {
    do {
      if(buffer.size() < 5) { // minimum size of a command
        printf("Too small buffer: %zu\n", buffer.size());
        return;
      }
			
      unsigned int command_size = buffer.getUI32(1); // peek
      if(buffer.size() < command_size) {
        printf("Buffer has only %zu bytes, and the command consists of: %u bytes.\n", buffer.size(), command_size);
        return;
      }

      // when we arrive at this point we received a complete command
      CommandData deserialized;
      if(util.deserialize(buffer, deserialized, server.elements)) {
        printf("Correctly deserialized.\n");
        deserialized.connection = this;
        server.addTask(deserialized);
      }

      printf("Command size: %u\n", command_size);
			
    } while(buffer.size() > 0);
  }

  // SERVER CONNECTIONS
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ServerConnections::ServerConnections(Server& server)
    :server(server)
  {
  }

  ServerConnections::~ServerConnections() {
    mutex.lock();
    clear();
    for(std::vector<ServerConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
      delete *it;
    }
    connections.clear();
    mutex.unlock();
  }

  void ServerConnections::start() {
    thread.create(*this);
  }

  void ServerConnections::clear() {
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

  void ServerConnections::run() {
    while(true) {
			
      mutex.lock();
      {
        // SEND DATA TO ALL CLIENTS OF NECESSARY
        for(std::deque<ServerCommand>::iterator it = to_all_commands.begin(); it != to_all_commands.end(); ++it) {
          for(std::vector<ServerConnection*>::iterator sit = connections.begin(); sit != connections.end(); ++sit) {
            (*sit)->send((*it));
          }
        }
        to_all_commands.clear();
				
        // REMOVE ALL DISCONNECTED CLIENTS
        clear();

        // SEND DATA TO SPECIFIC CLIENTS
        for(std::deque<ServerCommand>::iterator it = to_one_commands.begin(); it != to_one_commands.end(); ++it) {
          ServerCommand& cmd = *it;
          cmd.con->send(cmd);
        }
        to_one_commands.clear();

        // RECEIVE DATA FROM CLIENTS
        for(std::vector<ServerConnection*>::iterator sit = connections.begin(); sit != connections.end(); ++sit) {
          (*sit)->read();
        }
      }
      mutex.unlock();
    }
  }
 
  void ServerConnections::sendToAll(ServerCommand& cmd) {
    addToAllCommand(cmd);
  }
	
  void ServerConnections::sendToOne(ServerConnection* con, ServerCommand& cmd) {
    cmd.con = con;
    addToOneCommand(cmd);
  }

  void ServerConnections::addConnection(ServerConnection* conn) {
    mutex.lock();
    connections.push_back(conn);
    mutex.unlock();
  }
	
  void ServerConnections::addToAllCommand(ServerCommand cmd) {
    mutex.lock();
    to_all_commands.push_back(cmd);
    mutex.unlock();
  }

  void ServerConnections::addToOneCommand(ServerCommand cmd) {
    mutex.lock();
    to_one_commands.push_back(cmd);
    mutex.unlock();
  }

  void ServerConnections::removeConnection(ServerConnection* conn) {
    remove_list.push_back(conn);
  }

  // SERVER 
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Server::Server(const string serverTitle, int port, int portBroadcaster, const char* ip)
    :port(port)
    ,connections(*this)
    ,title(serverTitle)
    ,broadcast_port(portBroadcaster)
    ,broadcast_socket(SOCK_DGRAM, 0)
  {
    if(ip != NULL) {
      this->ip.append(ip, strlen(ip)+1);
    }
  }

  Server::~Server() {
  }

  void Server::start() {
    // initialize broadcaster.
    broadcast_on = Timer::now() + 5000; 
    broadcast_socket.create();
    broadcast_socket.setBroadcast();
    broadcast_info.setIP("192.168.0.255"); // @todo get this info from sockaddr
    broadcast_info.setPort(broadcast_port);
	
    std::stringstream ss;
    ss << title << ":" << "192.168.0.195" << ":" << port;
    broadcast_message = ss.str();
	  
    // start thread.
    thread.create(*this);
  }

  void Server::run() {
    // initialize tcp
    if(!server_socket.create()) {
      printf("Error: cannot create socket.\n");
      ::exit(0);
    }
    if(!server_socket.setReUseAddress()) {
      printf("Error: cannot set the reuse socket option.\n");
      ::exit(0);
    }
    if(!server_socket.bind((ip.size() > 0) ? ip.c_str() : NULL, port)) {
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

  // @todo, this piece of code is kind of similar to the client.. maybe put it in ClientServerUtils (?)
  void Server::update() {
    // check if we need to broadcast our IP+port
    if(Timer::now() > broadcast_on) {
      printf("Sending: %s\n", broadcast_message.c_str());
      broadcast_on = Timer::now() + 1000;
      broadcast_socket.sendTo(broadcast_info, broadcast_message.c_str(), broadcast_message.size());
    }
		
    mutex.lock();
    for(std::vector<CommandData>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
      CommandData& cmd = *it;
      switch(cmd.name) {
      case BDATA_GET_SCHEME: {
        // Client is connected and asked for the scheme.
        printf("Create and send scheme.\n");
        createScheme(); 
        sendScheme(cmd.connection);
        break;
      }
      case BDATA_SLIDERF: {
        if(cmd.sliderf != NULL) {
          cmd.sliderf->setValue(cmd.sliderf_value);
          cmd.sliderf->needsRedraw();
        }
        break;
      };
      case BDATA_SLIDERI: {
        if(cmd.slideri != NULL) {
          cmd.slideri->setValue(cmd.slideri_value);
          cmd.slideri->needsRedraw();
        }
        break;
      }
      case BDATA_TOGGLE: {
        cmd.toggle->setValue(cmd.toggle_value);
        cmd.toggle->needsRedraw();
        break;
      }
      case BDATA_BUTTON: {
        cmd.element->setValue(NULL);
        break;
      }
      case BDATA_RADIO: {
        cmd.element->setValue((void*)&cmd.radio_value);
        break;
      }
      case BDATA_COLOR: {
        cmd.element->setValue((void*)cmd.color_value);
        break;
      }
      case BDATA_VECTOR: {
        cmd.element->setValue((void*)cmd.vector_value);
        break;
      }
      case BDATA_PADI:
      case BDATA_PADF: {
        cmd.element->setValue((void*)cmd.pad_value);
        break;
      }
      default: printf("Error: Server received an Unhandled client task. %d \n", cmd.name); break;
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
  }

  void Server::onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target, void* targetData) {
    if(event == BEVENT_VALUE_CHANGED) {
      printf(">> Value changed (%p).\n", target);
      ServerCommand cmd(BDATA_CHANGED);
      if(utils.serialize(buttons, target, cmd.buffer, targetData)) {
        connections.sendToAll(cmd);
      }
    }
  }
	
  // Create the scheme which is used to recreate the buttons gui on the other side.
  void Server::createScheme() {
    scheme.clear();

    // first all buttons which are not part of a panel
    for(std::vector<Buttons*>::iterator it = buttons_to_sync.begin(); it != buttons_to_sync.end(); ++it) {
      Buttons& b = **it;
      // >> general info
      scheme.addByte(BDATA_GUI);
      scheme.addUI32(b.x);
      scheme.addUI32(b.y);
      scheme.addUI32(b.w);
      scheme.addUI32(b.h);
      scheme.addString(b.title);
			
      // >> elements
      scheme.addUI16(b.getNumParents());
      for(int i = 0; i < b.elements.size(); ++i) {
        Element* el = b.elements[i];
        if(el->is_child) {
          continue;
        }
        scheme.addByte(el->type);
        scheme.addString(el->label);
        scheme.addFloat(el->col_hue);
        scheme.addFloat(el->col_sat);
        scheme.addFloat(el->col_bright);

        switch(el->type) {
        case BTYPE_SLIDER: { el->serializeScheme(scheme); break; }
        case BTYPE_TOGGLE: { el->serializeScheme(scheme); break; }
        case BTYPE_BUTTON: { el->serializeScheme(scheme); break; }
        case BTYPE_RADIO: { el->serializeScheme(scheme); break; }
        case BTYPE_COLOR: { el->serializeScheme(scheme); break;	}
        case BTYPE_VECTOR: { el->serializeScheme(scheme); break; }
        case BTYPE_PAD: { el->serializeScheme(scheme); break; }
        default:break;
        };
      }
    }
  }

  void Server::sendScheme(ServerConnection* con) {
    ServerCommand cmd(BDATA_SCHEME);
    cmd.setData(scheme.getPtr(), scheme.getNumBytes());
    connections.sendToOne(con, cmd);
  }
	
} // buttons
