#ifndef ROXLU_WEBM_H
#define ROXLU_WEBM_H

#include <assert.h>
#include <inttypes.h>
#include <vector>
#include <algorithm>


extern "C" {
#   include <uv.h>
}

#include <webm/EBML.h>
#include <webm/VPXEncoder.h>

#define TIMER_START uint64_t timer_time_start = uv_hrtime();
#define TIMER_PRINT(x) printf("[TIMER] %s : %lld\n", x, (uv_hrtime() - timer_time_start));

enum WebmPacketTypes {
  WPT_VIDEO,
  WPT_AUDIO
};

struct WebmPacket {
  unsigned char* buf;
  int nbytes;
  char type;
  int64_t timestamp;
};

void webm_vpx_write_cb(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void* user);
void webm_encoder_thread(void* user);

class Webm {
 public: 
  Webm(EBML* ebml);
  ~Webm();

  void setEBML(EBML* ebml);

  bool setup(VPXSettings cfg);

  bool initialize();
  bool shutdown();

  bool startThread();
  bool stopThread();

  void addVideoFrame(unsigned char* data, int nbytes);
  bool wantsNewVideoFrame();

 public:                                /* public functions because they're called by a callback */
  void freePackets();                   /* frees all packets in this->packets */
  bool open();
  bool close();

 private:
  bool openEBML();                      /* open the EBML stream */
  bool closeEBML();                     /* close the EBML stream */
  bool initializeThread();              /* initialize thread related members */

 public:
  EBML* ebml;

  VPXSettings settings;
  int64_t vid_num_frames;               /* number of encoded/added frames */
  vpx_img_fmt vid_fmt;                  /* video input pixel format */
  VPXEncoder vid_enc;                   /* video encoder; wrapper around libvpx */
  std::vector<WebmPacket*> packets;     /* contains video/audio packets that needs to be encoded */
  uint64_t vid_timeout;                 /* time out when we want an new video frame */
  uint64_t vid_millis_per_frame;        /* how many millis per frame */

 public:  
  /* threading */
  bool must_stop;
  uv_thread_t thread;
  uv_mutex_t stop_mutex;
  uv_mutex_t packets_mutex;
};

inline void Webm::setEBML(EBML* ebml) {
  this->ebml = ebml;
}

inline bool Webm::wantsNewVideoFrame() {
  uint64_t now = uv_hrtime() / 1000000;
  if(vid_timeout == 0) {
    vid_timeout = now + vid_millis_per_frame;
    return true;
  }
  
  if(now >= vid_timeout) {
    vid_timeout = now + vid_millis_per_frame; // we could accumulate the differente here
    return true;
  }

  return false;
}

inline void Webm::freePackets() {
  WebmPacket* pkt = NULL;
  for(std::vector<WebmPacket*>::iterator it = packets.begin(); it != packets.end(); ++it) {
    pkt = *it;
    delete[] pkt->buf;
    delete pkt;
  }
  packets.clear();

}

#endif
