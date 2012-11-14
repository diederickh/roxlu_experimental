#include <Simulation.h>

static void setcolor(unsigned char* pixels, int r, int g, int b, int w, int h) {
  for(int i = 0; i < w; ++i) {
    for(int j = 0; j < h; ++j) {
      int dx = (j * w * 3) + i * 3;
      pixels[dx + 0] = r;
      pixels[dx + 1] = g;
      pixels[dx + 2] = b;
    }
  }
}

Simulation::Simulation()
  :SimulationBase()
  ,vid(1024,768,640,480,25, true, false)
{

  // Create some test pixel data
  test_data = new unsigned char[vid.getInWidth() * vid.getInHeight() * 3];
  for(int j = 0; j < vid.getInHeight(); ++j) {
    for(int i = 0; i < vid.getInWidth(); ++i) {
      int dx = j * vid.getInWidth() * 3 + i * 3;
      test_data[dx + 0] = 0xFF;
      test_data[dx + 1] = 0xFF;
      test_data[dx + 2] = 0x00;
    }
  }

  // Setup the video recorder.
  vid.setIO(&vid_io);
  vid.open("bin/io_output.264");

  // write 75 frames
  for(int i = 0; i < 75; ++i) { 
    if(i == 30) {
      setcolor(test_data, 255,0,0,vid.getInWidth(), vid.getInHeight());
    }
    vid.addVideoFrame(test_data);
  }

  // close
  vid.close();
}

void Simulation::setup() {
  setWindowTitle("H264 recorder example");
}

void Simulation::update() {}
void Simulation::draw() {}
void Simulation::onMouseDown(int x, int y, int button) {}
void Simulation::onMouseUp(int x, int y, int button) {}
void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {}
void Simulation::onMouseMove(int x, int y) {}
void Simulation::onKeyDown(char key) {}
void Simulation::onKeyUp(char key) {}
