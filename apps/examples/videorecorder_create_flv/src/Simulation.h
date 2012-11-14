#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videorecorder/VideoRecorder.h>

/*

  This example shows how to record rgb frames to 
  raw h264 which you can later convert using avconv
  into a mp4/mov etc..

  To convert the generated file, you can use to following 
  command:

  ./avconv -i io_output.264 -vcodec copy out2.mov

 */

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
  VideoRecorder vid;
  VideoIOFLV vid_io;
  unsigned char* test_data;
};
