#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videorecorder/VideoRecorder.h>
#include <audio/Audio.h>

void audio_in_cb(const void* input, unsigned long nframes, void* user);

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
  PCMWriter pcm;
private:
  VideoRecorder vid;
  Audio audio;

};
