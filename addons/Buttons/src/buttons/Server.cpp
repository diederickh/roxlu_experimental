#include <buttons/Server.h>
#include <buttons/Client.h>
#include <buttons/Buttons.h>
#include <sstream>

namespace buttons {

  // BUTTON SERVER
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Server::Server(int port)
    :port(port)
    ,loop(NULL)
  {
    loop = uv_default_loop();
    if(!loop) {
      RX_ERROR("Cannot get default libuv loop for buttons server");
    }
    sock.data = this;
  }

  Server::~Server() {
    RX_ERROR("need to shutdown uv + remove all clients, free mem");
    clear();
  }
  
  bool Server::start() {
    if(!loop) {
      RX_ERROR("No loop found");
      return false;
    }

    int r = 0;
    r = uv_tcp_init(loop, &sock);
    BUTTONS_UV_ERR(r, 0, "uv_tcp_init failed", false);
    
    struct sockaddr_in baddr = uv_ip4_addr("0.0.0.0", port);
    r = uv_tcp_bind(&sock, baddr);
    BUTTONS_UV_ERR(r, 0,  "uv_tcp_bind failed", false);

    r = uv_listen((uv_stream_t*)&sock, 128, buttons_server_on_new_connection);
    BUTTONS_UV_ERR(r, 0, "uv_listen failed", false);

    return true;
  }

  void Server::update() {
    uv_run(loop, UV_RUN_NOWAIT);
  }

  void Server::removeConnection(Connection* c) {
    std::vector<Connection*>::iterator it = std::find(connections.begin(), connections.end(), c);
    if(it == connections.end()) {
      RX_ERROR("cannot find client to remove (?)");
      return;
    }

    connections.erase(it);
    delete c;
    c = NULL;
  }

  void Server::createScheme() {
    scheme.clear();

    for(std::vector<Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      Buttons& b = **it;
      scheme.addByte(BDATA_GUI);
      scheme.addUI32(b.x);
      scheme.addUI32(b.y);
      scheme.addUI32(b.w);
      scheme.addUI32(b.h);
      scheme.addUI32(b.id);
      scheme.addFloat(b.col_hue);
      scheme.addFloat(b.col_sat);
      scheme.addFloat(b.col_bright);
      scheme.addFloat(b.col_alpha);
      scheme.addString(b.title);
			
      scheme.addUI16(b.getNumParents());
      for(int i = 0; i < b.elements.size(); ++i) {
        Element* el = b.elements[i];
        if(el->is_child) {
          continue;
        }
        scheme.addByte(el->type);
        scheme.addUI32(el->id);
        scheme.addString(el->label);
        scheme.addFloat(el->col_hue);
        scheme.addFloat(el->col_sat);
        scheme.addFloat(el->col_bright);

        switch(el->type) {
          case BTYPE_SLIDER:   { el->serializeScheme(scheme); break; }
          case BTYPE_TOGGLE:   { el->serializeScheme(scheme); break; }
          case BTYPE_BUTTON:   { el->serializeScheme(scheme); break; }
          case BTYPE_RADIO:    { el->serializeScheme(scheme); break; }
          case BTYPE_COLOR:    { el->serializeScheme(scheme); break; }
          case BTYPE_VECTOR:   { el->serializeScheme(scheme); break; }
          case BTYPE_PAD:      { el->serializeScheme(scheme); break; }
          default:break;
        };
      }
    }
  }

  void Server::sendScheme(Connection* con) {
    createScheme();

    ServerCommand cmd(BDATA_SCHEME);
    cmd.setData(scheme.getPtr(), scheme.getNumBytes());
    con->sendCommand(cmd);
  }

  void Server::addButtons(Buttons& gui) {
    gui.addListener(this);
    buttons.push_back(&gui);

    for(std::vector<Element*>::iterator it = gui.elements.begin(); it != gui.elements.end(); ++it) {
      Element& el = **it;
      elements[gui.id][el.id] = *it;
    }
  }

  void Server::onEvent(ButtonsEventType event, const Buttons& buttons, const Element* target, void* targetData) {
    if(event == BEVENT_VALUE_CHANGED) {
      ServerCommand cmd(BDATA_CHANGED);
      if(util.serialize(buttons, target, cmd.buffer, targetData)) {
        sendToAll(cmd);
      }
    }
  }

  void Server::sendToAll(ServerCommand& cmd) {
    for(std::vector<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
      (*it)->sendCommand(cmd);
    }
  }

  void Server::clear() {
    for(std::vector<Connection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
      delete *it;
    }
    connections.clear();
  }

  // SERVER COMMAND
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ServerCommand::ServerCommand(char name)
    :name(name)
  {
  }
  
  void ServerCommand::setData(char* data, size_t nbytes) {
    buffer.addBytes(data, nbytes);
  }

  // CONNECTION ON SERVER
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Connection::Connection(Server& server)
    :server(server)
  {
    sock.data = this;
    shutdown_req.data = this;
  }

  Connection::~Connection() {
    RX_ERROR("Connection client connection!");
  }

  void Connection::parseBuffer() {
    do {
      // check minimum size of a command.
      if(buffer.size() < 5) { 
        RX_VERBOSE("Buffer is too small, must be at least 5 bytes for a command");
        return;
      }
      
      // peek, and check if the buffer contains a complete command
      unsigned int command_size = buffer.getUI32(1); // peek
      if(buffer.size() < command_size) {
        RX_VERBOSE("Buffer has only %zu bytes, and the command consists of: %u bytes.\n", buffer.size(), command_size);
        return;
      }

      // when we arrive at this point we received a complete command
      CommandData deserialized;
      if(util.deserialize(buffer, deserialized, server.elements)) {
        handleCommand(deserialized);
      }

    } while(buffer.size() > 0);
  }

  void Connection::handleCommand(CommandData& cmd) {
    switch(cmd.name) {
      case BDATA_GET_SCHEME: {
        server.sendScheme(this);
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
      default: {
        RX_ERROR("Error: Server received an Unhandled client task. %d \n", cmd.name); 
        break;
      }
    }
  }

  void Connection::sendCommand(ServerCommand& cmd) {
    unsigned int size = cmd.buffer.getNumBytes();
    assert(sizeof(size) == 4); // we assumed that a uint is 4 bytes...

    write(&cmd.name, 1);
    write((char*)&size, sizeof(size));
    write(cmd.buffer.getPtr(), cmd.buffer.getNumBytes());
  }

  void Connection::write(char* data, size_t nbytes) {
     uv_buf_t buf = uv_buf_init(data, nbytes);
    uv_write_t* wreq = new uv_write_t();
    wreq->data = this;

    int r = uv_write(wreq, (uv_stream_t*) &sock, &buf, 1, buttons_connection_on_write);
    if(r) {
      RX_ERROR("cannot uv_write(): %s", uv_strerror(uv_last_error(sock.loop)));
    }
  }

  // CONNECTION CALLBACKS
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void buttons_connection_on_write(uv_write_t* req, int status) {
    Connection* c = static_cast<Connection*>(req->data);
    delete req;

    if(status == -1) {
      RX_ERROR("connection disconnected (?) ");
      return;
    }
  }
  

  // SERVER CALLBACKS
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void buttons_server_on_new_connection(uv_stream_t* sock, int status) {
    Server* s = static_cast<Server*>(sock->data);
    if(status == -1) {
      RX_ERROR("cannot handle new connection");
      return;
    }

    Connection* c = new Connection(*s);
    int r = 0;
    r = uv_tcp_init(s->loop, &c->sock);
    if(r) {
      RX_ERROR("uv_tcp_init failed on client");
      delete c; 
      c = NULL;
      return ;
    }

    r = uv_accept(sock, (uv_stream_t*)&c->sock);
    if(r) {
      RX_ERROR("uv_accept failed on client");
      delete c; 
      c = NULL;
      return;
    }
    
    r = uv_read_start((uv_stream_t*)&c->sock, buttons_server_on_alloc, buttons_server_on_read);
    if(r) {
      RX_ERROR("uv_read_start failed on client");
      delete c; 
      c = NULL;
      return;
    }

    s->connections.push_back(c);
  }

  uv_buf_t buttons_server_on_alloc(uv_handle_t* handle, size_t nbytes) {
    char* buf = new char[nbytes];
    return uv_buf_init(buf, nbytes);
  }

  void buttons_server_on_read(uv_stream_t* sock, ssize_t nread, uv_buf_t buf) {
    Connection* c = static_cast<Connection*>(sock->data);

    if(nread < 0) {
      int r = uv_read_stop(sock);
      if(r) {
        RX_ERROR("error shutting down client. %s", uv_strerror(uv_last_error(sock->loop)));
      }

      if(buf.base) {
        delete[] buf.base;
        buf.base = NULL;
      }

      uv_err_t err = uv_last_error(sock->loop);
      if(err.code != UV_EOF) {
        c->server.removeConnection(c);
        delete c;
        c = NULL;
        return;
      }
 
      r = uv_shutdown(&c->shutdown_req, sock, buttons_server_on_shutdown);
      if(r) {
        RX_ERROR("error shutting down client. %s", uv_strerror(uv_last_error(sock->loop)));
        delete c;
        c = NULL;
        return;
      }
 
      return;
    }

    c->buffer.addBytes(buf.base, nread);
    c->parseBuffer();

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }
  }
  
  void buttons_server_on_shutdown(uv_shutdown_t* req, int status) {
    Connection* c = static_cast<Connection*>(req->data);
    uv_close((uv_handle_t*)&c->sock, buttons_server_on_close);
  }

  void buttons_server_on_close(uv_handle_t* handle) {
    Connection* c = static_cast<Connection*>(handle->data);
    c->server.removeConnection(c);
  }


  // CLIENT <--> SERVER BUFFER UTILS
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ClientServerUtils::ClientServerUtils() {
  }

  ClientServerUtils::~ClientServerUtils() {
  }
	
  // Serializes data into a buffer after receiving a value changed event.
  bool ClientServerUtils::serialize(const Buttons& buttons, const Element* target, ButtonsBuffer& result, void* targetData) {
    unsigned int buttons_id = buttons.id;
    unsigned int element_id = target->id;
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
        const Toggle* toggle = static_cast<const Toggle*>(target);
        result.addUI32(buttons_id);
        result.addUI32(element_id);
        result.addByte(toggle->value == true ? 1 : 0);
        break;
      }
      case BTYPE_BUTTON: {
        int* button_id = (int*)targetData;
        result.addUI32(buttons_id);
        result.addUI32(element_id);
        result.addUI32(*button_id);
        break;
      }
      case BTYPE_RADIO: {
        int* selected = (int*)targetData;
        result.addUI32(buttons_id);
        result.addUI32(element_id);
        result.addUI32(*selected);
        break;
      }
      case BTYPE_COLOR: {
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
        RX_ERROR("Error: unhandled sync type: %d\n", target->type); 
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

        if(result.element == NULL) {
          RX_ERROR("We received an event for an unknown element: %d, buttons: %d", result.element_id, result.buttons_id);
          return false;
        }

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
            Toggle* toggle = static_cast<Toggle*>(result.element);
            result.toggle = toggle;
            result.toggle_value = (buffer.consumeByte() == 1);
            result.name = BDATA_TOGGLE;
            return true;
          }
          case BTYPE_BUTTON: {
            result.button_value = buffer.consumeUI32();
            result.name = BDATA_BUTTON;
            return true;
          }
          case BTYPE_RADIO: {
            result.radio_value = buffer.consumeUI32();
            result.name = BDATA_RADIO;
            return true;
          }
          case BTYPE_COLOR: {
            result.name = BDATA_COLOR;
            result.color_value[0] = buffer.consumeUI32();
            result.color_value[1] = buffer.consumeUI32();
            result.color_value[2] = buffer.consumeUI32();
            result.color_value[3] = buffer.consumeUI32();
            return true;
          }
          case BTYPE_VECTOR: {
            result.name = BDATA_VECTOR;
            result.vector_value[0] = buffer.consumeFloat();
            result.vector_value[1] = buffer.consumeFloat();
            return true;
          }
          case BTYPE_PAD: {
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

} // buttons
