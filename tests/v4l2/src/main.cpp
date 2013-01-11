#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include "V4L2Capture.h"

V4L2Capture cap;

void sig_handler(int signum) {
  printf("SIG HANDLER.\n");
  cap.closeDevice();
  exit(0);
}

int main() {
  printf("Video4Linux2 Test\n");
  
  //  signal(SIGINT, sig_handler);


  cap.setup();
  cap.listDevices();
  if(cap.openDevice(0)) {
      cap.startCapture();
      int num = 100000000;
      for(int i = 0; i < num; ++i) {
        //        printf("%d/%d\n", i, num);
        cap.update();

      }
      cap.closeDevice();
  }
  return 0;
}
