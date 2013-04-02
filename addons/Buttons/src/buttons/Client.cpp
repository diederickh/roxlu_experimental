#include <buttons/Client.h>

namespace buttons { 

  Client::Client(std::string host, std::string port)
    :port(port)
    ,host(host)
    ,loop(NULL)
  {
    loop = uv_default_loop();
    sock.data = this;
    resolver_req.data = this;
    connect_req.data = this;
    shutdown_req.data = this;
    timer_req.data = this;
  }

  Client::~Client() {
    clear();
  }

  bool Client::connect() {
    int r = uv_tcp_init(loop, &sock);
    if(r) {
      RX_ERROR("uv_tcp_init failed");
      return false;
    }

    struct addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
 
    r = uv_getaddrinfo(loop, &resolver_req, buttons_client_on_resolved, 
                       host.c_str(), port.c_str(), &hints);
 
    if(r) {
      RX_ERROR("cannot uv_tcp_init(): %s", uv_strerror(uv_last_error(loop)));
      return false;
    }
    
    return true;
  }

  void Client::reconnect() {
    clear(); // might be redundant (as it's done in the shutdown)
    int r = uv_timer_init(loop, &timer_req);
    if(r) {
      RX_ERROR("uv_time_init() failed. cannot reconnect");
      return;
    }
    
    uv_timer_start(&timer_req, buttons_client_on_reconnect_timer, 1000, 0);
  }

  void Client::update() {
    uv_run(loop, UV_RUN_NOWAIT);

    for(std::map<unsigned int, buttons::Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      it->second->update();
    }
  }

  void Client::draw() {
    for(std::map<unsigned int, buttons::Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      it->second->draw();
    }
  }

  void Client::onMouseMoved(int x, int y) {
    for(std::map<unsigned int, buttons::Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      it->second->onMouseMoved(x,y);
    }
  }

  void Client::onMouseUp(int x, int y) {
    for(std::map<unsigned int, buttons::Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      it->second->onMouseUp(x,y);
    }
  }

  void Client::onMouseDown(int x, int y) {
    for(std::map<unsigned int, buttons::Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      it->second->onMouseDown(x,y);
    }
  }

  void Client::parseBuffer() {
    do {
      if(buffer.size() < 5) { // each command must contain an ID (1 bytes) + size (4 bytes)
        return;
      }

      unsigned int command_size = buffer.getUI32(1); // peek the command size
      if(buffer.size() < command_size) {
        RX_VERBOSE("buffer not complete - we need more bytes, received: %ld but need %d", buffer.size(), command_size);
        return;
      }

      CommandData deserialized;
      if(util.deserialize(buffer, deserialized, elements)) {
        handleCommand(deserialized);
      }

    } while (buffer.size() > 0);
  }

  void Client::onEvent(ButtonsEventType event,
                       const Buttons& buttons, 
                       const Element* target, 
                       void* targetData)
  {
    if(event == BEVENT_VALUE_CHANGED) {
      ButtonsBuffer buffer;
      if(util.serialize(buttons, target, buffer, targetData)) {
        CommandData data;
        data.name = BDATA_CHANGED;
        data.buffer.addByte(data.name);
        data.buffer.addUI32(buffer.getNumBytes());
        data.buffer.addBytes(buffer.getPtr(), buffer.getNumBytes());
        write(data.buffer.getPtr(), data.buffer.getNumBytes());
      }
    }
  }

  void Client::operator()(unsigned int dx) {
    printf("Button pressed: %u \n", dx);
  }

  void Client::handleCommand(CommandData& cmd) {
    switch(cmd.name) {
      case BDATA_SCHEME: {
        parseScheme(cmd); 
        break;
      }
      case BDATA_SLIDERI: {
        cmd.slideri->setValue(cmd.slideri_value);
        cmd.slideri->needsRedraw();
        break;
      }
      case BDATA_SLIDERF: {
        cmd.sliderf->setValue(cmd.sliderf_value);
        cmd.sliderf->needsRedraw();  
        break;
      }
      case BDATA_TOGGLE: { 
        cmd.toggle->setValue(cmd.toggle_value);
        cmd.toggle->needsRedraw();
        break;
      }
      case BDATA_RADIO: {
        cmd.element->setValue((void*)&cmd.radio_value);
        cmd.element->needsRedraw();
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
      default: printf("Error: Unhandled in command.\n"); break;
    };
  }

  void Client::parseScheme(CommandData& cmd) {
    while(cmd.buffer.size() > 0) {
      char scheme_cmd = cmd.buffer.consumeByte();
      switch(scheme_cmd) {
        case BDATA_GUI: {
          unsigned int x = cmd.buffer.consumeUI32();
          unsigned int y = cmd.buffer.consumeUI32();
          unsigned int w = cmd.buffer.consumeUI32();
          unsigned int h = cmd.buffer.consumeUI32();
          unsigned int buttons_id = cmd.buffer.consumeUI32();
          float col_hue = cmd.buffer.consumeFloat();
          float col_sat = cmd.buffer.consumeFloat();
          float col_bright = cmd.buffer.consumeFloat();
          float col_alpha = cmd.buffer.consumeFloat();
          std::string title = cmd.buffer.consumeString();
          unsigned short num_els = cmd.buffer.consumeUI16();

          // STORE AT ID
          buttons::Buttons* gui = new Buttons(title, w);
          gui->addListener(this);
          gui->setPosition(x,y);
          gui->setColor(col_hue, col_sat, col_bright, col_alpha);
          gui->id = buttons_id;
          buttons[buttons_id] = gui;

          // PARSE ALL ELEMENTS
          for(int i = 0; i < num_els; ++i) {

            // COMMON/SHARED VALUES
            char el_type = cmd.buffer.consumeByte();
            unsigned int element_id = cmd.buffer.consumeUI32();

            std::string label = cmd.buffer.consumeString();
            float col_hue = cmd.buffer.consumeFloat();
            float col_sat = cmd.buffer.consumeFloat();
            float col_bright = cmd.buffer.consumeFloat();

            switch(el_type) {
              case BTYPE_SLIDER: {
                int value_type = cmd.buffer.consumeByte();

                if(value_type == Slider<float>::SLIDER_FLOAT) {
                  float value = cmd.buffer.consumeFloat();
                  float minv = cmd.buffer.consumeFloat();
                  float maxv = cmd.buffer.consumeFloat();
                  float stepv = cmd.buffer.consumeFloat();
                  float* dummy_value = new float;
                  value_floats.push_back(dummy_value);

                  // CREATE FLOAT SLIDER
                  Sliderf* sliderp = &gui->addFloat(label, *dummy_value).setMin(minv).setMax(maxv).setStep(stepv);
                  sliderp->setValue(value);
                  gui->setColor(col_hue);

                  sliderp->id = element_id;
                  elements[buttons_id][element_id] = sliderp;
                }
                else if(value_type == Slider<float>::SLIDER_INT) {
                  int value = cmd.buffer.consumeI32();
                  int minv = cmd.buffer.consumeI32();
                  int maxv = cmd.buffer.consumeI32();
                  int stepv = cmd.buffer.consumeI32();
                  int* dummy_value = new int;
                  value_ints.push_back(dummy_value);

                  Slideri* slideri = &gui->addInt(label, *dummy_value).setMin(minv).setMax(maxv).setStep(stepv);
                  slideri->setValue(value);
                  gui->setColor(col_hue);

                  slideri->id = element_id;
                  elements[buttons_id][element_id] = slideri;
                }
                break;
              }
              case BTYPE_TOGGLE: {
                bool value = cmd.buffer.consumeByte() == 1;
                bool* dummy_value = new bool;
                *dummy_value = value;
                value_bools.push_back(dummy_value);
                Toggle* toggle = &gui->addBool(label, *dummy_value);
                gui->setColor(col_hue);
                
                toggle->id = element_id;
                elements[buttons_id][element_id] = toggle;
                break;
              }
              case BTYPE_BUTTON: {
                printf("TODO: we need to fix the callback of a button. we changed from a functor() to a plain C-style callback\n");
                /*
                unsigned int button_id = cmd.buffer.consumeUI32();
                Button<Client>* button =  &gui->addButton<Client>(label, button_id, this);

                button->id = element_id;
                elements[buttons_id][element_id] = button;
                gui->setColor(col_hue);
                */
                break;
              }
              case BTYPE_RADIO: {
                int* dummy_value = new int;
                value_ints.push_back(dummy_value);
                unsigned int button_id = cmd.buffer.consumeUI32();
                unsigned int selected_value = cmd.buffer.consumeUI32();
                unsigned int num_options = cmd.buffer.consumeUI32();
                *dummy_value = selected_value;

                std::vector<std::string> options;
                for(int i = 0; i < num_options; ++i) {
                  std::string option_title = cmd.buffer.consumeString();
                  options.push_back(option_title);
                }

                // create radio + set selected value
                Radio<Client>* radio = &gui->addRadio<Client>(label, button_id, this, options, *dummy_value);

                radio->id = element_id;
                elements[buttons_id][element_id] = radio;
                gui->setColor(col_hue);
                break;
              };
              case BTYPE_COLOR: {
                unsigned int hue = cmd.buffer.consumeUI32();
                unsigned int sat = cmd.buffer.consumeUI32();
                unsigned int light = cmd.buffer.consumeUI32();
                unsigned int alpha = cmd.buffer.consumeUI32();

                float* col_ptr = new float[4];
                value_float_arrays.push_back(col_ptr);

                ColorPicker* picker = &gui->addColor(label, col_ptr);
                picker->hue_slider.setValue(hue);
                picker->sat_slider.setValue(sat);
                picker->light_slider.setValue(light);
                picker->alpha_slider.setValue(alpha);
                gui->setColor(col_hue);

                picker->id = element_id;
                elements[buttons_id][element_id] = picker;
                break;
              }
              case BTYPE_VECTOR: {
                float vx = cmd.buffer.consumeFloat();
                float vy = cmd.buffer.consumeFloat();
                float* vec_ptr = new float[2];
                vec_ptr[0] = vx;
                vec_ptr[1] = vy;
                value_float_arrays.push_back(vec_ptr);
                Vector<float>* vec = &gui->addVec2f(label, vec_ptr);
                vec->setValue((void*)vec_ptr);

                vec->id = element_id;
                elements[buttons_id][element_id] = vec;

                gui->setColor(col_hue);
                break;
              }
              case BTYPE_PAD: {
                char value_type = cmd.buffer.consumeByte();
                if(value_type == PAD_FLOAT) {
                  float minx = cmd.buffer.consumeFloat();
                  float maxx = cmd.buffer.consumeFloat();
                  float miny = cmd.buffer.consumeFloat();
                  float maxy = cmd.buffer.consumeFloat();
                  float px = cmd.buffer.consumeFloat();
                  float py = cmd.buffer.consumeFloat();
                  float* pad_ptr = new float[2];
                  pad_ptr[0] = px;
                  pad_ptr[1] = py;
                  value_float_arrays.push_back(pad_ptr);
                  Pad<float>* padf = &gui->addFloat2(label, pad_ptr);
                  padf->id = element_id;
                  elements[buttons_id][element_id] = padf;
                  padf->setPercentages(px, py);
                  gui->setColor(col_hue);
                }
                else if(value_type == PAD_INT) {
                  unsigned int minx = cmd.buffer.consumeUI32();
                  unsigned int maxx = cmd.buffer.consumeUI32();
                  unsigned int miny = cmd.buffer.consumeUI32();
                  unsigned int maxy = cmd.buffer.consumeUI32();
                  float px = cmd.buffer.consumeFloat();
                  float py = cmd.buffer.consumeFloat();
                  int* pad_ptr = new int[2];
                  pad_ptr[0] = px;
                  pad_ptr[1] = py;
                  value_int_arrays.push_back(pad_ptr);
                  Pad<int>* padi = &gui->addInt2(label, pad_ptr);
                  padi->id = element_id;
                  elements[buttons_id][element_id] = padi;
                  padi->setPercentages(px, py);
                  gui->setColor(col_hue);
                }
                break;
              }
              default: {
                printf("Error: Unhandled scheme type: %d\n", el_type);
                break;
              }}
          }
          break; // fror now we break after one el.
        }
        default:break;
      }
    }
  }

  void Client::sendCommand(CommandData cmd) {
    write(cmd.buffer.getPtr(), cmd.buffer.getNumBytes());
  }

  void Client::write(char* data, size_t nbytes) {
    uv_buf_t buf = uv_buf_init(data, nbytes);
    uv_write_t* wreq = new uv_write_t();
    wreq->data = this;

    int r = uv_write(wreq, (uv_stream_t*)&sock, &buf, 1, buttons_client_on_write);
    if(r) {
      RX_ERROR("uv_write() to server failed.");
    }
  }

  void Client::getScheme() {
    CommandData data;
    data.name = BDATA_GET_SCHEME;
    data.buffer.addByte(BDATA_GET_SCHEME);
    data.buffer.addUI32(0);
    sendCommand(data);
  }

  void Client::clear() {
    for(std::vector<float*>::iterator it = value_floats.begin(); it != value_floats.end(); ++it) {delete *it;  }
    value_floats.clear();

    for(std::vector<int*>::iterator it = value_ints.begin(); it != value_ints.end(); ++it) {delete *it;  }
    value_ints.clear();

    for(std::vector<bool*>::iterator it = value_bools.begin(); it != value_bools.end(); ++it) {delete *it;  }
    value_bools.clear();

    for(std::vector<float*>::iterator it = value_float_arrays.begin(); it != value_float_arrays.end(); ++it) {delete[] *it;  }
    value_float_arrays.clear();

    for(std::vector<int*>::iterator it = value_int_arrays.begin(); it != value_int_arrays.end(); ++it) {delete[] *it;  }
    value_int_arrays.clear();

    for(std::map<unsigned int, buttons::Buttons*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
      delete it->second; // buttons object deletes all elements
    }

    elements.clear();
    buttons.clear();
    buffer.clear();

    RX_WARNING("---- ALL CLEAR -------");
  }

  // CLIENT CALLBACKS
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void buttons_client_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
    RX_VERBOSE("resolved with status: %d", status);
    Client* c = static_cast<Client*>(req->data);
    if(status == -1) {
      RX_ERROR("cannot resolve(): %s", uv_strerror(uv_last_error(c->loop)));
      c->reconnect();
      return;
    }

    char ip[17] = {0};
    uv_ip4_name((struct sockaddr_in*)res->ai_addr, ip, 16);
    RX_VERBOSE("resolved server: %s", ip);
 
    int r = uv_tcp_connect(&c->connect_req, &c->sock, 
                           *(struct sockaddr_in*)res->ai_addr, 
                           buttons_client_on_connect);
 
    uv_freeaddrinfo(res);
  }

    
  void buttons_client_on_connect(uv_connect_t* req, int status) {
    Client* c = static_cast<Client*>(req->data);
    if(status == -1) {
      RX_ERROR("cannot connect: %s", uv_strerror(uv_last_error(c->loop)));
      c->reconnect();
      return;
    }
    
    int r = uv_read_start((uv_stream_t*)&c->sock, buttons_client_on_alloc, buttons_client_on_read);
    if(r) {
      RX_ERROR("uv_read_start() failed %s", uv_strerror(uv_last_error(c->loop)));
      return;
    }

    c->getScheme();
  }

  void buttons_client_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf) {
    RX_VERBOSE("Received data from server, :%ld bytes", nbytes);
    Client* c = static_cast<Client*>(handle->data);

    if(nbytes < 0) {
      int r = uv_read_stop(handle);
      if(r) {
        RX_ERROR("error uv_read_stop on client. %s", uv_strerror(uv_last_error(handle->loop)));
      }

      if(buf.base) {
        delete[] buf.base;
        buf.base = NULL;
      }

      uv_err_t err = uv_last_error(handle->loop);
      if(err.code != UV_EOF) {
        RX_ERROR("disconnected from server, but not correctly!");
        return;
      }

      r = uv_shutdown(&c->shutdown_req, handle, buttons_client_on_shutdown);
      if(r) {
        RX_ERROR("error shutting down client. %s", uv_strerror(uv_last_error(handle->loop)));
        delete c;
        c = NULL;
        return;
      }
       
      RX_ERROR("------- NEED TO RECONNECT TO THE SERVER ------------- ");
      return;
      
    }

    c->buffer.addBytes(buf.base, nbytes);
    c->parseBuffer();

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }
  }

  uv_buf_t buttons_client_on_alloc(uv_handle_t* handle, size_t nbytes) {
    RX_ERROR("we need to free this buffer!");
    char* buf = new char[nbytes];
    return uv_buf_init(buf, nbytes);
  }

  void buttons_client_on_write(uv_write_t* req, int status) {
    delete req;
  }

  void buttons_client_on_shutdown(uv_shutdown_t* req, int status) {
    Client* c = static_cast<Client*>(req->data);
    uv_close((uv_handle_t*)&c->sock, buttons_client_on_close);
  }

  void buttons_client_on_close(uv_handle_t* handle) {
    Client* c = static_cast<Client*>(handle->data);
    c->clear();
    c->reconnect();
  }

  void buttons_client_on_reconnect_timer(uv_timer_t* handle, int status) {
    Client* c = static_cast<Client*>(handle->data);
    if(status == -1) {
      RX_ERROR("error shutting down client. %s", uv_strerror(uv_last_error(handle->loop)));
      return;
    }
    
    RX_VERBOSE("reconnecting");
    c->connect();
  }

} // buttons
