#include <Simulation.h>

static void on_option_click(int id, void* user) {
  RX_VERBOSE("Clicked on option: %d", id);
}

static void on_button_clicked(int id, void* user) {
  RX_VERBOSE("On button clicked: %d", id);
}

Simulation::Simulation()
  :SimulationBase()
  ,gui("settings", 300, 400)
  ,server(5050)
{
}

void Simulation::operator()(unsigned int dx) {
  RX_VERBOSE("Pressed/Selected: %d", dx);
}

void Simulation::setup() {
  setWindowTitle("Buttons server");

  gui.addBool("Enabled", test_enabled);

  gui.addFloat("Force", test_slider).setMin(0.0f).setMax(1.0f);
  gui.addColor("Color", test_color);
  gui.addInt("Num elements", test_int).setMin(0).setMax(50);
  gui.addInt2("Direction", test_vec).setX(0,100).setY(0,100);
  gui.addFloat2("Velocity", test_vel).setX(0.0f, 40.0f).setY(0.0f, 60.0f);
  gui.addRectanglei("Rectangle I", test_recti).setX(0,100).setY(0,500); // not yet networked
  gui.addRectanglef("Rectangle F", test_rectf).setX(0.0f,200.0f).setY(10.0f,300.0f);  // not yet networked
  gui.addVec2f("Vec2f", test_accelf);

  vector<string> options;
  options.push_back("Option 1");
  options.push_back("Option 2");
  options.push_back("Option 3");

  //gui.addRadio<Simulation>("Options", 1, this, options, test_option); 
  gui.addButton("Button", 0, on_button_clicked, this);
  
  gui.setColor(0.3f);
  //  gui.load();
  server.addButtons(gui.getButtons("settings"));
  server.start();
}


void Simulation::update() {
  gui.update();
  server.update();
}

void Simulation::draw() {
  glClearColor(test_color[0], test_color[1], test_color[2], test_color[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();

  gui.draw();
}

void Simulation::onMouseDown(int x, int y, int button) {
  gui.onMouseDown(x,y);
}

void Simulation::onMouseUp(int x, int y, int button) {
  gui.onMouseUp(x,y);
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  gui.onMouseMoved(x,y);
}

void Simulation::onMouseMove(int x, int y) {
  gui.onMouseMoved(x,y);
}

void Simulation::onChar(int ch) {

  if(ch == 's') {
    gui.save();
  }
  else if(ch == 'l') {
    gui.load(); 
  }

}

void Simulation::onKeyDown(int key) {
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

