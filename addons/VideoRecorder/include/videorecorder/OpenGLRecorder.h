#ifndef ROXLU_OPENGL_RECORDERH
#define ROXLU_OPENGL_RECORDERH

#include <roxlu/Roxlu.h>
#include <vector>
#include <string>

class VideoIOFLV;
class VideoRecorder;

class OpenGLRecorderWorker : public Runnable {
 public:
  OpenGLRecorderWorker();
  ~OpenGLRecorderWorker();
  void run();
  void copyImage(unsigned char* pixels);
  void copyAudio(void* data, size_t nbytes);
  void flush();
  void clear();
 public:
  RingBuffer video_buffer;
  RingBuffer audio_buffer;
  bool must_flush;
  VideoRecorder* rec;
  Mutex mutex;
  size_t num_bytes_in_image;
  std::vector<unsigned char*> images;
  unsigned char* tmp_pixel_buffer;
};

class OpenGLRecorder {
 public:
  OpenGLRecorder(float fps = 30.0f);
  ~OpenGLRecorder();
  void setOutputSize(int w, int h);
  void open(const std::string& filepath, int outW = 0, int outH = 0);
  void grabFrame();
  void addAudioFrame(void* data, size_t nbytes);
  void close();
  void setFPS(const float fs);
 private:
  OpenGLRecorderWorker worker;
  Thread worker_thread;
  VideoIOFLV* io_flv;
  VideoRecorder* rec;
  unsigned char* pixels;
  unsigned char* dest_pixels;
  int w; 
  int h;
  int out_w;
  int out_h;
  float fps;
  rx_uint64 time_per_frame;
  rx_uint64 grab_timeout;
};
#endif
