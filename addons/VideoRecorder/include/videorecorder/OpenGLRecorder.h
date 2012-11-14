#ifndef ROXLU_OPENGL_RECORDERH
#define ROXLU_OPENGL_RECORDERH

class VideoIOFLV;
class VideoRecorder;
class OpenGLRecorder {
 public:
  OpenGLRecorder();
  ~OpenGLRecorder();
  void setOutputSize(int w, int h);
  void open(const char* filepath, int outW = 0, int outH = 0);
  void grabFrame();
  void close();
 private:
  VideoIOFLV* io_flv;
  VideoRecorder* rec;
  unsigned char* pixels;
  unsigned char* dest_pixels;
  int w; 
  int h;
  int out_w;
  int out_h;
};
#endif
