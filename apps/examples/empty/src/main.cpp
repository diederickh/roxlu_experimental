#include <Simulation.h>
#include <iostream>

int main() {

  // setup GLFW
  if(!rx_glfw_init()) {
    exit(EXIT_FAILURE);
  }

  // create window.
  GLFWwindow* win = rx_glfw_create_window(1280, 720, "Simulation", NULL, NULL);
  if(!win) {
    exit(EXIT_FAILURE);
  }

  // create simulation app.
  Simulation sim;
  rx_glfw_setup_simulation(&sim, win);

  while(!glfwWindowShouldClose(win)) {

    sim.update();
    sim.draw();

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
};

