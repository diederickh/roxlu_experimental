#include <glfw_wrapper/SimulationBase.h>
#include <buttons/Buttons.h>
#include <roxlu/Roxlu.h>
#include <flv/FLVScreenRecorder.h>
#include <pbd/PBD.h>

#include "RParticles.h"
#include "RDebugDrawer.h"

// Using a simple fixed timestep; we could use a more advanced version, see:
// http://gafferongames.com/game-physics/fix-your-timestep/

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
  void onChar(int ch); 
  void onKeyDown(int key);
  void onKeyUp(int key);
  void onWindowClose();
 private:
  EasyCam cam;
  FPS fps;

  RParticles ps;
  RDebugDrawer rdrawer;

  // fixed timestep
  float dt;
  double last_timestep;
  double time_accum;

  FLVScreenRecorder recorder;
};
