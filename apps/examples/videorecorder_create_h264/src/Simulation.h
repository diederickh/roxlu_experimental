#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videorecorder/VideoRecorder.h>
#include <libyuv.h>

class Rec {
 public:
  Rec(int inW, int inH, int outW, int outH, int fps, const std::string filepath);
  void addFrame(unsigned char* pixels);
  void close();
 private:
  int fps;
  int in_width;
  int in_height;
  int out_width;
  int out_height;
  int num_frames;
  std::string filepath;
  FILE* fp;
  SwsContext* sws;
  x264_param_t params;
  x264_t* encoder;
  x264_picture_t pic_in;
  x264_picture_t pic_out;
  x264_nal_t* nals;
};

class Simulation : public SimulationBase {
public:
  Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y, int dx, int dy, int button);
  void onMouseMove(int x, int y);
  void onKeyDown(char key);
  void onKeyUp(char key);
private:
  EasyCam cam;
  VideoRecorder vid;
  VideoIOH264 vid_io;
   //VideoIOFLV vid_io;
  unsigned char* test_data;
  Rec rec;
};
