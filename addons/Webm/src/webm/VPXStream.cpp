#include <webm/VPXStream.h>

// BUFFER 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXBuffer::VPXBuffer()
  :read_dx(0)
{
}

VPXBuffer::~VPXBuffer() {
  reset();
}

void VPXBuffer::addBytes(char* b, size_t nbytes) {
  std::copy(b, b+nbytes, std::back_inserter(data));
}

void VPXBuffer::clear() {
  data.clear();
}

void VPXBuffer::reset() {
  clear();
  read_dx = 0;
}

char* VPXBuffer::ptr() {
  return (char*)&data[0];
}

char* VPXBuffer::readPtr() {
  return ptr() + read_dx;
}

size_t VPXBuffer::size() {
  return data.size();
}


void VPXBuffer::flush(int n) {
  data.erase(data.begin(), data.begin() + n);
  read_dx -= n;
  if(read_dx < 0) {
    read_dx = 0;
  }
}

void VPXBuffer::print() {
  unsigned char* p = (unsigned char*) ptr();
  for(int i = 0; i < size(); ++i) {
    printf("%02X ", *(p+i));
    if(i > 0 && i % 40 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}


void VPXBuffer::write(char* b, size_t nbytes) {
  std::copy(b, b+nbytes, std::back_inserter(data));
}

void VPXBuffer::wu8(uint8_t b) {
  data.push_back(b);
}

void VPXBuffer::wu16(uint16_t b) {
  wu8(b >> 8);
  wu8(b);
}

void VPXBuffer::wu32(uint32_t b) {
  wu8(b >> 24);
  wu8(b >> 16);
  wu8(b >> 8);
  wu8(b);
};

uint8_t VPXBuffer::ru8() {
  return data[read_dx++];
}

uint16_t VPXBuffer::ru16() {
  uint16_t tmp = 0;
  memcpy((char*)&tmp, (char*)&data[read_dx], sizeof(tmp));
  uint16_t result = (tmp << 8) | (tmp >> 8);
  read_dx += 2;
  return result;
}

uint32_t VPXBuffer::ru32() {
  uint32_t tmp = 0;
  memcpy((char*)&tmp, (char*)&data[read_dx], sizeof(tmp));
  uint32_t result = ((tmp >> 24) & 0x000000FF) | ((tmp << 24) * 0xFF000000) | ((tmp >> 8) & 0x0000FF00) | ((tmp << 8) & 0x00FF0000);
  read_dx += 4;
  return result;
}

uint8_t VPXBuffer::pu8(int dx) {
  size_t tmp_dx = read_dx;
  read_dx += dx;
  uint8_t r = ru8();
  read_dx = tmp_dx;
  return r;
}

uint16_t VPXBuffer::pu16(int dx) {
  size_t tmp_dx = read_dx;
  read_dx += dx;
  uint16_t r = ru16();
  read_dx = tmp_dx;
  return r;
}

uint32_t VPXBuffer::pu32(int dx) {
  size_t tmp_dx = read_dx;
  read_dx += dx;
  uint32_t r = ru32();
  read_dx = tmp_dx;
  return r;
}

// IN STREAM
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXInStream::VPXInStream(std::string host, std::string port) 
  :loop(NULL)
  ,host(host)
  ,port(port)
  ,proto_version(0)
  ,cb_decode(NULL)
  ,cb_user(NULL)
{
  loop = uv_default_loop();
  sock.data = this;
  shutdown_req.data = this;
  resolver_req.data = this;
  connect_req.data = this;
  timer_req.data = this;
}

VPXInStream::~VPXInStream() {
  cb_decode = NULL;
  cb_user = NULL;
  proto_version = 0;
  loop = NULL;
  host = "";
  port = "";
}

bool VPXInStream::connect(vpx_read_cb decodeCB, void* user) {
  cb_decode = decodeCB;
  cb_user = user;
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

  r = uv_getaddrinfo(loop, &resolver_req, vpx_client_on_resolved, 
                     host.c_str(), port.c_str(), &hints);

  if(r) {
    RX_ERROR("cannot uv_tcp_init(): %s", uv_strerror(uv_last_error(loop)));
    return false;
  }

  return true;
}

void VPXInStream::update() {
  uv_run(loop, UV_RUN_NOWAIT);
}

void VPXInStream::parseBuffer() {
  do {
    uint8_t cmd = buffer.pu8();
    uint32_t nbytes = buffer.pu32(1);

    if(buffer.size() < (nbytes + 5)) {
      RX_VERBOSE("buffer not big enough, we have: %ld but need: %d", buffer.size(), nbytes);
      return;
    }

    buffer.flush(5);

    switch(cmd) {
      case VPX_CMD_SETTINGS: {

        proto_version = buffer.ru8();
        settings.in_w = buffer.ru16();
        settings.in_h = buffer.ru16();
        settings.out_w = buffer.ru16();
        settings.out_h = buffer.ru16();
        settings.fps = buffer.ru16();

        buffer.flush(nbytes);

        RX_VERBOSE("version: %d, in_w: %d, in_h: %d, out_w: %d, out_h: %d, fps: %d, bytes left: %ld", 
                    proto_version, settings.in_w, settings.in_h, 
                    settings.out_w, settings.out_h,
                    settings.fps, buffer.size());

        if(settings.in_w != 0 && settings.in_h != 0 
           && settings.out_w != 0 && settings.out_h != 0)
          {
            settings.cb_read = cb_decode;
            settings.cb_user = cb_user;
            decoder.setup(settings);
        }
        else {
          RX_ERROR("The received encoder settings don't contain a valid frame size");
        }
        break;
      };
      case VPX_CMD_FRAME: {
        decoder.decodeFrame((unsigned char*)buffer.readPtr(), nbytes);
        buffer.flush(nbytes);
        break;
      }
      default: {
        RX_WARNING("Unhandled command in VPXInstream: %d", cmd); 
        break;
      }
    }
  } while(buffer.size() > 5);
}

void VPXInStream::reconnect() {
  clear(); 

  int r = uv_timer_init(loop, &timer_req);
  if(r) {
    RX_ERROR("uv_timer_init() failed. cannot reconnect");
    return;
  }

  uv_timer_start(&timer_req, vpx_client_on_reconnect_timer, 1000, 0);
  
}

void VPXInStream::clear() {
  buffer.clear();
}

// OUT STREAM
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXOutStream::VPXOutStream(int port) 
  :port(port)
  ,loop(NULL)
  ,is_setup(false)
  ,num_frames(0)
{
  RX_VERBOSE("VPXOutStream()");
  sock.data = this;
}

VPXOutStream::~VPXOutStream() {
  is_setup = false;
  num_frames = 0;
  loop = NULL;
  port = 0;
}

bool VPXOutStream::setup(VPXSettings cfg) {
  RX_ERROR("we need to enforce the AV_PIX_FMT_BGRA format as we're grabbing in this format");
  settings = cfg;

  if(!grabber.setup(settings, vpx_server_on_grabber_cb, this)) {
    RX_ERROR("cannot setup grabber");
    return false;
  }

  settings.cb_user = this;
  settings.cb_write = vpx_server_on_vpx_write;
  if(!encoder.setup(settings)) {
    RX_ERROR("cannot setup encoder");
    return false;
  }

  if(!encoder.initialize()) {
    RX_ERROR("cannot initailize encoder");
    return false;
  }

  is_setup = true;
  return true;
}

bool VPXOutStream::start() {
  if(!is_setup) {
    RX_ERROR("first call setup()");
    return false;
  }

  loop = uv_default_loop();
  if(!loop) {
    RX_ERROR("Cannot create loop");
    return false;
  }

  int r = 0;
  r = uv_tcp_init(loop, &sock);
  VPX_UV_ERR(r, 0, "uv_tcp_init failed", false);

  struct sockaddr_in baddr = uv_ip4_addr("0.0.0.0", port);
  r = uv_tcp_bind(&sock, baddr);
  VPX_UV_ERR(r, 0, "uv_tcp_bind failed", false);

  r = uv_listen((uv_stream_t*)&sock, 128, vpx_server_on_new_connection);
  VPX_UV_ERR(r, 0, "uv_listen failed", false);

  return true;
}

void VPXOutStream::update() {
  if(!is_setup) {
    RX_ERROR("not setup!");
    return;
  }
  uv_run(loop, UV_RUN_NOWAIT);
}

void VPXOutStream::grabFrame() {
  grabber.grabFrame();
}

void VPXOutStream::removeConnection(VPXConnection* c) {
  std::vector<VPXConnection*>::iterator it = std::find(connections.begin(), connections.end(), c);
  if(it != connections.end()) {
    connections.erase(it);
  }
}

void VPXOutStream::writeHeader(VPXConnection* c) {
  RX_VERBOSE("send header to client: in_w: %d, in_h: %d, fps: %d ", settings.out_w, settings.out_h, settings.fps);
  VPXBuffer buf;
  buf.wu8(1); // version
  buf.wu16(settings.out_w);
  buf.wu16(settings.out_h);
  buf.wu16(settings.in_w);
  buf.wu16(settings.in_h);
  buf.wu16(settings.fps);
  c->writeCommand(VPX_CMD_SETTINGS, buf);
}

void VPXOutStream::writeToAllConnections(VPXCommands c, char* buf, size_t nbytes) {
  VPXBuffer tmp;
  tmp.write(buf, nbytes);
  for(std::vector<VPXConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    VPXConnection& con = **it;
    con.writeCommand(c, tmp);
  }
}

// ScreenGrabber
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXScreenGrabber::VPXScreenGrabber()
  :num_bytes(0)
  ,read_dx(0)
  ,write_dx(1)
  ,millis_per_frame(0)
  ,frame_timeout(0)
  ,is_setup(false)
  ,cb_grabber(NULL)
  ,cb_user(NULL)
{

}

VPXScreenGrabber::~VPXScreenGrabber() {
  is_setup = false;
  read_dx = 0;
  write_dx = 1;
  millis_per_frame = 0;
  num_bytes = 0;
  frame_timeout = 0;
  cb_grabber = NULL;
  cb_user = NULL;
}

bool VPXScreenGrabber::setup(VPXSettings cfg, vpx_screengrabber_cb grabCB, void* grabUser) {
  cb_grabber = grabCB;
  cb_user = grabUser;
  settings = cfg;
  millis_per_frame = (1.0f/settings.fps) * 1000;

  num_bytes = settings.in_w * settings.in_h * 4; // we grab in BGRA, gpu default
  if(num_bytes == 0) {
    RX_ERROR("Unsupported image format");
    return false;
  }

  glGenBuffers(VPX_SCREENGRABBER_FBOS, pbos);
  for(int i = 0; i < VPX_SCREENGRABBER_FBOS; ++i) {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
    glBufferData(GL_PIXEL_PACK_BUFFER, num_bytes, NULL, GL_STREAM_READ);
  }
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  is_setup = true;
  return true;
}

void VPXScreenGrabber::grabFrame() {
  if(!is_setup) {
    RX_ERROR("not setup, cannot grab");
    return;
  }

  if(!cb_grabber) {
    return;
  }

  if(!mustGrabFrame()) {
    return;
  }
  
  read_dx = (read_dx + 1) % VPX_SCREENGRABBER_FBOS;
  write_dx = (read_dx + (VPX_SCREENGRABBER_FBOS-2)) % VPX_SCREENGRABBER_FBOS;
  
  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[write_dx]);
  glReadPixels(0,0,settings.in_w, settings.in_h, GL_BGRA, GL_UNSIGNED_BYTE, 0);

  glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[read_dx]);
  unsigned char* ptr = (unsigned char*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
  if(ptr) {
    cb_grabber(ptr, num_bytes, cb_user);
  }

  glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

bool VPXScreenGrabber::mustGrabFrame() {
  int64_t now = uv_hrtime();
  if(!frame_timeout) {
    frame_timeout = now + (millis_per_frame * 1000000);
    return true;
  }
  else if(now > frame_timeout) {
    frame_timeout = now + (millis_per_frame * 1000000);
    return true;
  }
  else {
    return false;
  }
}


// Connection
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXConnection::VPXConnection(VPXOutStream& os) 
  :out_stream(os)
  ,keyframe_sent(false)
{
  sock.data = this;
  shutdown_req.data = this;
}

VPXConnection::~VPXConnection() {
}

void VPXConnection::parseBuffer() {
  RX_VERBOSE("Parse connection buffer - for now we don't really do anything here....");
}

void VPXConnection::writeCommand(VPXCommands cmd, VPXBuffer& buf) {
  /* the command */
  char c = cmd;
  write(&c, 1);

  /* data size */
  VPXBuffer tmp;
  tmp.wu32(buf.size());
  write(tmp.ptr(), tmp.size());

  /* the data */
  write(buf.ptr(), buf.size());
}

void VPXConnection::write(char* data, size_t nbytes) {
  uv_buf_t buf = uv_buf_init(data, nbytes);
  uv_write_t* req = new uv_write_t();
  req->data = this;

  int r = uv_write(req, (uv_stream_t*)&sock, &buf, 1, vpx_connection_on_write);
  if(r) {
    RX_ERROR("cannot uv_write(): %s", uv_strerror(uv_last_error(sock.loop)));
  }
}

// GRABBER
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void vpx_server_on_grabber_cb(unsigned char* pixels, size_t nbytes, void* user) {
  VPXOutStream* s = static_cast<VPXOutStream*>(user);
  s->num_frames++;
  s->encoder.encode(pixels, s->num_frames);
}

// VPX (VPXOutStream)
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void vpx_server_on_vpx_write(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void* user) {
  VPXOutStream* s = static_cast<VPXOutStream*>(user);
  
  // debug 
  // ------
#if 0
  if(s->connections.size() > 0) {
    unsigned char* p = (unsigned char*)pkt->data.frame.buf;
    for(int i = 0; i < pkt->data.frame.sz; ++i) {
      if(i > 0 && i % 10 == 0) {
        printf("\n");
      }
      printf("%02X ", *(p+i));
    }
    printf("\n");
  }
  else {
    printf("NOT SENT!\n---------------------------------------\n");
    unsigned char* p = (unsigned char*)pkt->data.frame.buf;
    for(int i = 0; i < pkt->data.frame.sz; ++i) {
      if(i > 0 && i % 10 == 0) {
        printf("\n");
      }
      printf("%02X ", *(p+i));
    }
    printf("\n--------------------------\n");
  }
  // ----
#endif

  VPXBuffer tmp;
  tmp.write((char*)pkt->data.frame.buf, pkt->data.frame.sz);
  if(pkt->data.frame.flags & VPX_FRAME_IS_KEY) {
    for(std::vector<VPXConnection*>::iterator it = s->connections.begin(); it != s->connections.end(); ++it) {
      VPXConnection& con = **it;
      con.writeCommand(VPX_CMD_FRAME, tmp);
      con.keyframe_sent = true;
    }
  }
  else {
    for(std::vector<VPXConnection*>::iterator it = s->connections.begin(); it != s->connections.end(); ++it) {
      VPXConnection& con = **it;
      if(con.keyframe_sent) {
        con.writeCommand(VPX_CMD_FRAME, tmp);
      }
    }
  }
}

// UV CONNECTION
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void vpx_connection_on_write(uv_write_t* req, int status) {
  VPXConnection* c = static_cast<VPXConnection*>(req->data);
  delete req;

  if(status == -1) {
    RX_ERROR("error while trying to uv_write: %s", uv_strerror(uv_last_error(c->sock.loop)));
  }

}

// UV SERVER
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void vpx_server_on_new_connection(uv_stream_t* sock, int status) {
  VPXOutStream* s = static_cast<VPXOutStream*>(sock->data);
  if(status == -1) {
    RX_ERROR("cannot handle connection (?)");
    return;
  }

  VPXConnection* con = new VPXConnection(*s);
  int r = 0;

  r = uv_tcp_init(s->loop, &con->sock);
  if(r) {
    RX_ERROR("uv_tcp_init failed for new connection.");
    delete con;
    con = NULL;
    return;
  }

  r = uv_accept(sock, (uv_stream_t*)&con->sock);
  if(r) {
    RX_ERROR("uv_accept failed for new connection.");
    delete con;
    con = NULL;
    return;
  }

  r = uv_read_start((uv_stream_t*)&con->sock, vpx_server_on_alloc, vpx_server_on_read);
  if(r) {
    RX_ERROR("uv_read_start failed for new connection.");
    delete con;
    con = NULL;
    return;
  }

  s->writeHeader(con);
  s->encoder.forceKeyFrame();
  s->connections.push_back(con);
}

uv_buf_t vpx_server_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];
  if(!buf) {
    RX_ERROR("ERROR - OUT OF MEMORY IN VPX STREAM");
  }
  return uv_buf_init(buf, nbytes);
}

void vpx_server_on_read(uv_stream_t* sock, ssize_t nread, uv_buf_t buf) {
  VPXConnection* c = static_cast<VPXConnection*>(sock->data);

  /* disconnected */
  if(nread < 0) {

    int r = uv_read_stop(sock);
    if(r) {
      RX_ERROR("failed to uv_read_stop() after client disconnected: %s", uv_strerror(uv_last_error(sock->loop)));
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

    uv_err_t err = uv_last_error(sock->loop);
    if(err.code != UV_EOF) {
      c->out_stream.removeConnection(c);
      delete c;
      c = NULL;
      return;
    }


    r = uv_shutdown(&c->shutdown_req, sock, vpx_server_on_shutdown);
    if(r) {
      RX_ERROR("failed to uv_shutdown() after client disconnected: %s", uv_strerror(uv_last_error(sock->loop)));
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

void vpx_server_on_shutdown(uv_shutdown_t* req, int status) {
  VPXConnection* c = static_cast<VPXConnection*>(req->data);
  uv_close((uv_handle_t*)&c->sock, vpx_server_on_close);
}

void vpx_server_on_close(uv_handle_t* handle) {
  VPXConnection* c = static_cast<VPXConnection*>(handle->data);
  c->out_stream.removeConnection(c);
  delete c;
}


// UV CLIENT 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void vpx_client_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
  VPXInStream* s = static_cast<VPXInStream*>(req->data);

  if(status == -1) {
      RX_ERROR("cannot resolve(): %s RECONNECT!", uv_strerror(uv_last_error(s->loop)));
      s->reconnect();
      return;
  }
  
  char ip[17] = {0};
  uv_ip4_name((struct sockaddr_in*)res->ai_addr, ip, 16);
  RX_VERBOSE("resolved server: %s", ip);
 
  int r = uv_tcp_connect(&s->connect_req, &s->sock, 
                         *(struct sockaddr_in*)res->ai_addr, 
                         vpx_client_on_connect);
 
  uv_freeaddrinfo(res);
}

void vpx_client_on_connect(uv_connect_t* req, int status) {

  VPXInStream* s = static_cast<VPXInStream*>(req->data);
  if(status == -1) {
    RX_ERROR("cannot connect: %s RECONNECT!", uv_strerror(uv_last_error(s->loop)));
    s->reconnect();
    return;
  }
    
  int r = uv_read_start((uv_stream_t*)&s->sock, vpx_client_on_alloc, vpx_client_on_read);
  if(r) {
    RX_ERROR("uv_read_start() failed %s", uv_strerror(uv_last_error(s->loop)));
    return;
  }
}

void vpx_client_on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {
  VPXInStream* s = static_cast<VPXInStream*>(handle->data);

  if(nread < 0) {
    int r = uv_read_stop(handle);
    if(r) {
      RX_ERROR("failed to uv_read_stop() after client disconnected: %s", uv_strerror(uv_last_error(handle->loop)));
    }

    uv_err_t err = uv_last_error(handle->loop);
    if(err.code != UV_EOF) {
      RX_ERROR("disconnected from server, but not correctly!");
      return;
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

    r = uv_shutdown(&s->shutdown_req, handle, vpx_client_on_shutdown);
    if(r) {
      RX_ERROR("error shutting down client. %s", uv_strerror(uv_last_error(handle->loop)));
      delete s;
      s = NULL;
      return;
    }
    RX_ERROR("------- NEED TO RECONNECT TO THE SERVER ------------- ");
    return;
      
  }

  s->buffer.addBytes(buf.base, nread);
  s->parseBuffer();

  if(buf.base) {
    delete[] buf.base;
    buf.base = NULL;
  }
}

void vpx_client_on_write(uv_write_t* req, int status) {
  RX_ERROR("- need to implement + free req ");
}

void vpx_client_on_shutdown(uv_shutdown_t* req, int status) {
  VPXInStream* s = static_cast<VPXInStream*>(req->data);
  uv_close((uv_handle_t*)&s->sock, vpx_client_on_close);
}

void vpx_client_on_close(uv_handle_t* handle) {
  VPXInStream* s = static_cast<VPXInStream*>(handle->data);
  s->clear();
  s->reconnect();
}

void vpx_client_on_reconnect_timer(uv_timer_t* handle, int status) {
  VPXInStream* s = static_cast<VPXInStream*>(handle->data);
  if(status == -1) {
    RX_ERROR("error in reconnect timer. %s", uv_strerror(uv_last_error(handle->loop)));
    return;
  }
  s->connect(s->cb_decode, s->cb_user);
}

uv_buf_t vpx_client_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];
  if(!buf) {
    RX_ERROR("Cannot allocate memory for uv_buf_t, client stream");
  }
  return uv_buf_init(buf, nbytes);
}
