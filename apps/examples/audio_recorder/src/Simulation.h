#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <audio/Audio.h>
#include <audio/MP3Writer.h>

void audio_in_cb(const void* input, unsigned long nframes, void* user);

class Simulation : public SimulationBase {
public:
  Simulation();
  ~Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y, int dx, int dy, int button);
  void onMouseMove(int x, int y);
  void onKeyDown(char key);
  void onKeyUp(char key);

 public:
  PCMWriter pcm;                 /* we create a pcm + mp3 file */
  MP3Writer mp3;                 /* we create a pcm + mp3 file - encodes audio and passes it into the writer  */
  MP3FileWriter mp3_file_writer; /* this helps us to write the encode mp3 data to a file */

private:
  Audio audio;

};
