#include <webm/Webm.h>

void webm_vpx_write_cb(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void* user) {

  Webm& webm = *(static_cast<Webm*>(user));

  EBMLSimpleBlock b;
  b.track_number = 1; // hardcoded: track 1 = video 
  //b.timestamp = pkt->data.frame.pts;
  b.timestamp = pts; // uv_hrtime();
  b.flags = 0;
  if(pkt->data.frame.flags & VPX_FRAME_IS_KEY) {
    b.flags |= 0x80;
  }
  b.nbytes = pkt->data.frame.sz;
  b.data = (char*)pkt->data.frame.buf;

  webm.ebml->addSimpleBlock(b);
}

void webm_encoder_thread(void* user) {

  Webm& webm = *(static_cast<Webm*>(user));
  std::vector<WebmPacket*> work_packets;
  WebmPacket* pkt = NULL;
  bool stop = false;

  webm.open();

  while(true) {

    uv_mutex_lock(&webm.packets_mutex);
    std::copy(webm.packets.begin(), webm.packets.end(), std::back_inserter(work_packets));
    webm.packets.clear();
    uv_mutex_unlock(&webm.packets_mutex);
    
    for(std::vector<WebmPacket*>::iterator it = work_packets.begin(); it != work_packets.end(); ++it) {
      pkt = *it;
      webm.vid_enc.encode(pkt->buf, pkt->timestamp);

      delete[] pkt->buf;
      delete pkt;
      pkt = NULL;
    }

    work_packets.clear();

    uv_mutex_lock(&webm.stop_mutex);
    stop = webm.must_stop;
    uv_mutex_unlock(&webm.stop_mutex);

    if(stop) {
      webm.freePackets();
      break;
    }
  }

  webm.close();
}


Webm::Webm(EBML* ebml) 
  :ebml(ebml)
  ,vid_num_frames(0)
  ,vid_timeout(0)
  ,vid_millis_per_frame(0)
  ,must_stop(false)
{
}

Webm::~Webm() {
  shutdown();
}

bool Webm::setup(VPXSettings cfg) {

  settings = cfg;
  settings.cb_write = webm_vpx_write_cb;
  settings.cb_user = this;

  vid_millis_per_frame = (1.0f/settings.fps) * 1000;
  vid_timeout = 0;

  bool r = vid_enc.setup(settings);
  if(!r) {
    return false;
  }
  return true;
  return true;
}

bool Webm::initialize() {
  if(!initializeThread()) {
    return false;
  }

  if(!vid_enc.initialize()) {
    return false;
  }

  return true;
}

bool Webm::open() {

  if(!openEBML()) {
    return false;
  }

  return true;
}

bool Webm::openEBML() {

  assert(ebml != NULL);

  EBMLHeader h;
  h.ebml_version = 1;
  h.ebml_read_version = 1;
  h.ebml_max_id_length = 4;
  h.ebml_max_size_length = 8;
  h.doctype = "webm";
  h.doctype_version = 2;
  h.doctype_read_version = 2;

  if(!ebml->open(h)) {
    RX_ERROR("canot open ebml stream");
    return false;
  }

  EBMLSegmentInfo info;
  info.title = "roxlu";
  info.muxing_app = "roxlu"; 
  info.writing_app = "roxlu";
  info.timecode_scale = 1000000;
  
  if(!ebml->openSegment(info)) {
    RX_ERROR("cannot write segment");
    return false;
  }

  EBMLTrack vid_track;
  vid_track.type = EBML_TRACK_TYPE_VIDEO;
  vid_track.number = 1;
  vid_track.uid = ebml->generateUID(vid_track.number);
  vid_track.codec_id = "V_VP8";
  vid_track.vid_pix_width = settings.out_w;
  vid_track.vid_pix_height = settings.out_h;

  std::vector<EBMLTrack> tracks;
  tracks.push_back(vid_track);
  
  if(!ebml->openTracks(tracks)) {
    RX_ERROR("Cannot write tracks");
    return false;
  }

  return true;
}


bool Webm::close() {
  closeEBML();
  vid_num_frames = 0;
  return true;
}

bool Webm::closeEBML() {
  return ebml->close();
}


bool Webm::startThread() {
  uv_thread_create(&thread, webm_encoder_thread, this);
  return true;
}

bool Webm::stopThread() {

  uv_mutex_lock(&stop_mutex);
  must_stop = true;
  uv_mutex_unlock(&stop_mutex);

  uv_thread_join(&thread);

  return true;
}

bool Webm::shutdown() {

  vid_millis_per_frame = 0;
  vid_timeout = 0;

  stopThread();

  return true;
}

bool Webm::initializeThread() {

  if(uv_mutex_init(&stop_mutex) != 0) {
    return false;
  }

  if(uv_mutex_init(&packets_mutex) != 0) {
    return false;
  }

  return true;
}

void Webm::addVideoFrame(unsigned char* data, int nbytes) {

  {
    bool stop = false;
    uv_mutex_lock(&stop_mutex);
    stop = must_stop;
    uv_mutex_unlock(&stop_mutex);

    if(stop) {
      return;
    }
  }

  {
    WebmPacket* p = new WebmPacket();
    if(!p) {
      RX_ERROR("out of mem");
      return;
    }

    p->buf = new unsigned char[nbytes];
    if(!p->buf) {
      RX_ERROR("out of mem");
      return;
    }

    memcpy(p->buf, data, nbytes);
    //p->timestamp = vid_num_frames;
    p->timestamp = uv_hrtime() / 1000000;
    p->type = WPT_VIDEO;
    p->nbytes = nbytes;

    uv_mutex_lock(&packets_mutex);
    packets.push_back(p);
    uv_mutex_unlock(&packets_mutex);

    vid_num_frames++;
  }
}

