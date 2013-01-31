/* 
 Encoding with x264 and muxing into FLV
 --------------------------------------
 Usage:
 --------------------------------------
 // setup the flv bitstream
 FLV flv;
 flv.setCallbacks(write_flv, rewrite_flv, flush_flv, close_flv, this); // callbacks
 
 // setup av
 AV av;
 av.setFLV(flv); 
 av.setupVideo(640, 480, 640, 480, 60, AV_FMT_RGB24);
 av.setupAudio(1, 44100, 512, AV_FMT_INT16); 
 av.initialize();
 
 // add new audio frames as soon as they arrive:
 av.addAudioFrame(data, number_of_frames); // number of frames is probably 512 (in this example)

 // start the AV thread
 av.start();

 // add new video frames when necessary:
 if(av.wantsNewVideoFrame()) {
    av.addVideoFrame(pixels); 
 )

 // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 //                     SUPER IMPORTANT!
 // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 // BECAUSE ENCODING IS DONE IN A SEPARATE THREAD YOU NEED TO 
 // MAKE SURE THAT YOU CALL: av.waitForEncodingThreadToFinish()
 // IN YOU DESTRUCTOR! 
 // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 av.waitForEncodingThreadToFinish();
 


 Dev info:
 --------
 The order in which you create x264 and an flv is (which is al nicely wrapped in this class):
 - x264: setup x264 parameters
 - x264: create encoder
 - flv: open flv and write flv-header (FLV ..)
 - flv: write params (onMetaData)
 - x264: encode headers
 - flv: writeAvcSequenceHeader() 
   ...
 - flv: writeVideoPacket() / writeAudioPacket() <-- repeated
 */

#ifndef ROXLU_AV_H
#define ROXLU_AV_H

#include <inttypes.h>

extern "C" {
#include <x264.h>
#include <libswscale/swscale.h>
#include <lame/lame.h>
}

#include <roxlu/Roxlu.h>
#include <stdio.h>
#include <vector>
#include <flv/FLV.h>


#define MP3_BUFFER_SIZE 8192

enum AVVideoFormat {
  AV_FMT_RGB24,
  AV_FMT_BGRA32
};

enum AVAudioFormat {
  AV_FMT_FLOAT32,
  AV_FMT_INT16
};

enum AVPacketType {
  AV_VIDEO,
  AV_AUDIO
};

struct AVPacket {
  rx_uint64 timestamp;
  int type; 
  size_t write_index; 
  size_t num_bytes;
  int num_frames; // num audio frames, or always 1 for a video frame
};

struct AVPacketSorter {
  bool operator()(const AVPacket* a, const AVPacket* b) {
    return a->timestamp < b->timestamp;
  }
};

class AV : public Runnable {
 public: 
  AV();
  ~AV();
  bool setupVideo(int inWidth, int inHeight, int outWidth, int outHeight, double fps, AVVideoFormat videoFmt);
  bool setupAudio(int numChannels, int sampleRate, int maxSamplesPerFrame, AVAudioFormat audioFmt);
  bool initialize();
  void run();  // threaded function
  bool wantsNewVideoFrame();
  void addAudioFrame(const void* data, int nframes);
  void addVideoFrame(const void* data);
  void encodeAudioPacket(AVPacket* p);
  void encodeVideoPacket(AVPacket* p);
  void setFLV(FLV* flv);
  void reset();

  void start();
  void stop();

  void waitForEncodingThreadToFinish();
  void setVerticalFlip(bool flip);  // enable or disable vertical flip of video input, must be called before setupVideo()  (handy when recording downloaded pixels from opengl)

 private:
  bool initializeVideo();
  bool initializeAudio();
  bool setupX264();
  void printX264Params(x264_param_t* p);
  void printX264Headers(x264_nal_t* nal); // prints result of x264_encoder_headers
  void printAVPixelFormat(AVPixelFormat p);
  FLVAudioSampleRate audioSampleRateToFLVSampleRate(int rate);
  AVPixelFormat videoFormatToAVPixelFormat(AVVideoFormat f);

 private:

  /* audio members */
  AVAudioFormat audio_fmt;
  int audio_samplerate; // 44100, 16000
  int audio_num_channels; // 1, 2
  int audio_bytes_per_sample; // num bytes for one sample e.g. sizeof(short)
  int audio_bytes_per_frame; // audio_num_channels * audio_bytes_per_sample * audio_max_samples_per_frame
  int audio_max_samples_per_frame; // often when you record audio you often get X-samples back in your callback function. e.g. I get 512 samples by default in my port audio input stream callback function
  bool audio_is_buffer_ready; // is set to true when you can start reading from the audio buffer
  rx_uint64 audio_total_samples; // total number of samples received
  rx_uint64 audio_time_started; // used 
  unsigned char audio_tmp_in_buffer[MP3_BUFFER_SIZE * 2]; // interleaved, directly from ring buffer
  unsigned char audio_tmp_in_buffer_left[MP3_BUFFER_SIZE]; // left channel
  unsigned char audio_tmp_in_buffer_right[MP3_BUFFER_SIZE]; // right channel
  unsigned char audio_tmp_out_buffer[MP3_BUFFER_SIZE]; // encoded mp3 data
  lame_global_flags* audio_lame_flags; // mp3 encoding flags
  
  /* video members */
  AVVideoFormat vid_fmt;
  int vid_in_w; 
  int vid_in_h;
  int vid_out_w;
  int vid_out_h;
  int vid_num_channels; // how many color channels (3 = rgb, 4 = rgba)
  double vid_fps;
  double vid_millis_per_frame;
  int vid_total_frames; // number of frames added to the encodersi
  int vid_bytes_per_frame; // num bytes for one image frame (w * h * bpp)
  rx_uint64 vid_timeout; // times out when we need a new video frame
  rx_uint64 vid_time_started; // time when we started adding frames
  bool vid_is_buffer_ready; // set to true when all buffers contain image data
  char* vid_tmp_buffer; // buffer for at least one image
  x264_param_t vid_params;
  x264_t* vid_encoder;
  x264_picture_t vid_pic_in;
  x264_picture_t vid_pic_out; // result from x264_encoder_encode(), we don't have to free this; only when using x264_picture_alloc
  SwsContext* vid_sws;
  bool vid_vflip; // flip video input

  /* muxer */
  bool is_initialized;

  FLV* flv;
  Mutex mutex;
  std::vector<AVPacket*> packets;
  RingBuffer audio_ring_buffer;
  RingBuffer video_ring_buffer;
  RingBuffer audio_work_buffer;
  RingBuffer video_work_buffer;

  /* thread */
  bool must_stop; // when called stop(), we stop our threaded function.
  Thread thread; // thread handle, see start(), stop
};

inline void AV::addVideoFrame(const void* data) {
  if(must_stop) {
    return;
  }
  if(audio_num_channels == 0 || audio_is_buffer_ready) {
    if(!vid_time_started) {
      vid_time_started = rx_millis();
    }

    mutex.lock();
    {
      AVPacket* p = new AVPacket();
      p->type = AV_VIDEO;
      p->timestamp = rx_millis() - vid_time_started;
      p->write_index = video_ring_buffer.getWriteIndex();
      p->num_bytes =  vid_bytes_per_frame;
      p->num_frames = 1;

      video_ring_buffer.write((char*)data, p->num_bytes);
      packets.push_back(p);

    }
    mutex.unlock();
  }
  vid_is_buffer_ready = true;
}

inline void AV::addAudioFrame(const void* data, int nframes) {
  if(must_stop) {
    return;
  }
  if(!vid_is_buffer_ready) {
    return;
  }

  audio_total_samples += nframes;

  mutex.lock();
  {
    if(!audio_time_started) {
      audio_time_started = rx_millis();
    }

    AVPacket* p = new AVPacket();
    p->type = AV_AUDIO;
    p->timestamp = rx_millis() - audio_time_started;
    p->write_index = audio_ring_buffer.getWriteIndex();
    p->num_bytes = nframes * audio_bytes_per_sample * audio_num_channels;
    p->num_frames = nframes;
    audio_ring_buffer.write((char*)data, p->num_bytes);
    packets.push_back(p);
  }
  mutex.unlock();

  audio_is_buffer_ready = true;
}


inline bool AV::wantsNewVideoFrame() {

  if(vid_timeout == 0) {
    vid_timeout = rx_millis() + vid_millis_per_frame;
    return true;
  }

  rx_uint64 now = rx_millis();
  if(vid_timeout <= now) {
    vid_timeout = now + vid_millis_per_frame;
    return true;
  }
  return false;
}

inline void AV::setFLV(FLV* f) {
  flv = f;
}

inline void AV::start() {
  thread.create(*this);
}

inline void AV::stop() {
  must_stop = true;
}

inline void AV::waitForEncodingThreadToFinish() {
  thread.join();
}

inline AVPixelFormat AV::videoFormatToAVPixelFormat(AVVideoFormat f) {
  switch(f) {
    case AV_FMT_RGB24: return PIX_FMT_RGB24; 
    case AV_FMT_BGRA32: return PIX_FMT_BGRA;
    default: return PIX_FMT_NONE;
  }
}


inline void AV::setVerticalFlip(bool flip) {
  vid_vflip = true;
}
#endif
